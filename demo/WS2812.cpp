#include <algorithm>
#include <cstdlib>
#include <vector>

#include "fragment.hpp"
#include "hardware/structs/systick.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include "util.hpp"
using std::vector;

// writes nLEDs RGB values from dataRGB
// note: WS2812 expects "GRB" word order (R: bit 15:8; G: bit 23:16; B: bit 7:0; unused: bit 31:25)
// time critical - don*t run from flash (risk of cache miss)
// replicate this function for all used GPIOs (152 bytes)
void __not_in_flash_func(driveLEDsGPIO0)(uint32_t *dataGRB, uint32_t nLEDs) {
    // see WS2812B datasheet
    const float t0H_s = 0.4e-6;
    const float t1H_s = 0.8e-6;
    const float t0L_s = 0.85e-6;
    const float t1L_s = 0.45e-6;

    // datasheet requires 50 us for reset but example hardware needed slightly more
    const float tReset_s = 60e-6;

    // 125 MHz was measured on the RP2040 board (12M crystal), 4 instructions per loop
    const float fSys_Hz = 125e6 / 4.0;
    const uint32_t nTics0H = (uint32_t)(t0H_s * fSys_Hz + 0.5);
    const uint32_t nTics1H = (uint32_t)(t1H_s * fSys_Hz + 0.5);
    const uint32_t nTics0L = (uint32_t)(t0L_s * fSys_Hz + 0.5);
    const uint32_t nTics1L = (uint32_t)(t1L_s * fSys_Hz + 0.5);
    const uint32_t nTicsReset = (uint32_t)(tReset_s * fSys_Hz + 0.5);

    // === write all LEDs ===
    uint32_t *p = dataGRB;
    for (uint32_t ixLED = nLEDs; ixLED != 0; --ixLED) {
        uint32_t v = *(p++);
        for (uint32_t ixBit = 24; ixBit != 0; --ixBit) {
            // === write one bit ===
            if (v & 0x00800000) {  // 1 bit
                for (int32_t ccount = nTics1H; ccount >= 0; --ccount)
                    gpio_put(0, 1);
                for (int32_t ccount = nTics1L - 10; ccount >= 0; --ccount)
                    gpio_put(0, 0);
            } else {  // 0 bit
                for (int32_t ccount = nTics0H; ccount >= 0; --ccount)
                    gpio_put(0, 1);
                for (int32_t ccount = nTics0L - 10; ccount >= 0; --ccount)
                    gpio_put(0, 0);
            }
            v <<= 1;
        }  // for bit
    }      // for ixLED

    // === drive reset pulse ===
    // note: not timing-sensitive if minimum duration is achieved.
    // could be removed from interrupts-off section and shared across all GPIOs
    // Note: GPIO state is already low at this point - the gpio_put() is redundant.
    for (int32_t ccount = nTicsReset; ccount >= 0; --ccount)
        gpio_put(0, 0);
}  // while forever

int main() {
    // === configure GPIO 0 ===
    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_slew_rate(0, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(0, GPIO_DRIVE_STRENGTH_12MA);
    gpio_put(0, 0);

    // === storage for LED color info ==
    const uint32_t nLEDs = 144;
    uint32_t LED[nLEDs];

    // === main loop ===
    uint32_t timestamp_ms = to_ms_since_boot(get_absolute_time());
    const uint32_t frameLength_ms = 10;  // 100 Hz refresh
    const float frameLength_s = (float)frameLength_ms / 1000.0f;
    const float accel = -2.0f;
    vector<fragment> frags;
    uint32_t blockedToTimestamp = timestamp_ms;
    while (1) {
        // === clear LED storage ===
        for (size_t ix = 0; ix < nLEDs; ++ix)
            LED[ix] = 0;

        // === remove expired frags ===
        frags.erase(
            std::remove_if(frags.begin(), frags.end(), [](auto frag) { return frag.expired(); }),
            frags.end());

        // === shoot a Roman Light ===
        if ((frags.size() < 6) && (blockedToTimestamp <= timestamp_ms)) {
            blockedToTimestamp = timestamp_ms + (uint32_t)util::frand(600.0f, 1700.0f);
            const float pos = 0;
            const float v0 = util::frand(1.6f, 2.01f);
            const float duration = util::frand(1.4f, 2.2f);
            uint32_t rgb;
            switch (rand() % 7) {
                case 0:
                    rgb = 0xFF0000;
                    break;
                case 1:
                    rgb = 0x00FF00;
                    break;
                case 2:
                    rgb = 0x0000FF;
                    break;
                case 3:
                    rgb = 0xFFFF00;
                    break;
                case 4:
                    rgb = 0x00FFFF;
                    break;
                case 5:
                    rgb = 0xFF00FF;
                    break;
                case 6:
                default:
                    rgb = 0xFFFFFF;
                    break;
            }

            float explodeAt = 9e9;
            if (util::frand(0.0f, 1.0f) > 0.9f)
                explodeAt = util::frand(0.9f, 1.4f);
            frags.emplace(frags.end(), pos, v0, rgb, duration, explodeAt);
        }

        // === process explosions ===
        size_t nFragsOrig = frags.size();
        for (size_t ix = 0; ix < nFragsOrig; ++ix)
            frags[ix].spawn(frags);  // careful: insertion would invalidate any vector::iterator

        // === render fireworks ===
        for (fragment &f : frags) {
            f.tick(frameLength_s, accel);
            f.renderGRB(LED, nLEDs);
        }

        // === send to hardware ===
        // note: an interrupt would cause bit errors due to the timing sensitive protocol.
        uint32_t intState = save_and_disable_interrupts();
        driveLEDsGPIO0(LED, nLEDs);
        restore_interrupts(intState);

        // === sleep until next update ===
        timestamp_ms += frameLength_ms;
        while (to_ms_since_boot(get_absolute_time()) < timestamp_ms) {
        }
    }  // while forever
}
