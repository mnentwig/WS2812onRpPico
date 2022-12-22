#pragma once
#include <cmath>
#include <vector>

#include "pico/stdlib.h"
#include "util.hpp"
using std::vector;
// fireworks simulator for programmable LED strip
class fragment {
   public:
    fragment(float pos, float velocity, uint32_t RGB, float duration, float explodeAt)
        : pos(pos),
          velocity(velocity),
          duration(duration),
          age(0),
          explodeAt(explodeAt),
          rgbOrig(RGB) {
        red = (RGB >> 16) & 0xFF;
        green = (RGB >> 8) & 0xFF;
        blue = RGB & 0xFF;
    }

    void renderGRB(uint32_t* LEDs, uint32_t nLEDs) {
        // === which LED? ===
        if (pos < 0) return;  // off-screen
        uint32_t posLED = std::floor(pos * (nLEDs - 1) + 0.5);
        if (posLED >= nLEDs) return;  // off-screen

        // === which color? ===
        if (age >= duration) return;
        float brightness = 1.0f - age / duration;
        if (brightness <= 0) return;
        brightness *= brightness;  // linear RGB value vs. logarithmic eye sensitivity

        uint32_t component;
        component = std::min(green * brightness + 0.5f, 255.0f);
        uint32_t GRB = component << 16;
        component = std::min(red * brightness + 0.5f, 255.0f);
        GRB |= component << 8;
        component = std::min(blue * brightness + 0.5f, 255.0f);
        GRB |= component;

        // === set LED ===
        *(LEDs + posLED) = GRB;  // controller end is bottom pos=0
        //  *(LEDs + nLEDs - 1 - posLED) = GRB; // reverse
    };

    void spawn(vector<fragment>& frags) {
        if (age < explodeAt) return;
        explodeAt = 9e9;  // large number: explode only once

        // === turn bright white with short duration ===
        red = 0xFF;
        green = 0xFF;
        blue = 0xFF;
        duration = age + 0.1f;

        // === create new fragments ===
        uint32_t nFrags = 3; //0.5f + util::frand(2.0f, 7.0f);
        for (uint32_t ixFrag = 0; ixFrag < nFrags; ++ixFrag) {
            const float v0 = util::frand(-0.1f, 2.5f) + velocity;
            const float duration = util::frand(1.4f, 2.2f);
#if 0
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
#endif
            frags.emplace(frags.end(), pos, v0, rgbOrig, duration, /*explodeAt*/ 9e9);
        }  // for frag
    }

    void tick(float deltaTime, float accel) {
        velocity += accel * deltaTime;
        pos += velocity * deltaTime;
        age += deltaTime;
    };

    bool expired() {
        return pos < 0;
    }

   protected:
    float pos;
    float velocity;
    float duration;
    float age;
    float explodeAt;
    float red;
    float green;
    float blue;
    uint32_t rgbOrig;
};
