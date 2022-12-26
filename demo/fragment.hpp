#pragma once
#include <cmath>
#include <vector>

#include "pico/stdlib.h"
#include "util.hpp"
using std::vector;
// fireworks simulator for programmable LED strip
class fragment {
   public:
    enum prio_e { EXHAUST,
                  EXPLSHELL,
                  SHELL,
                  EXPLOSION };

    fragment(prio_e prio, float pos, float velocity, uint32_t RGB, float duration, float explodeAt, float igniteAt)
        : prio(prio),
          pos(pos),
          velocity(velocity),
          duration(duration),
          age(0),
          explodeAt(explodeAt),
          igniteAt(igniteAt),
          rgbOrig(RGB) {
        red = (RGB >> 16) & 0xFF;
        green = (RGB >> 8) & 0xFF;
        blue = RGB & 0xFF;
    }

    void renderGRB(uint32_t* LEDs, uint32_t nLEDs) {
        // === which LED? ===
        if (pos < 0) return;         // off-screen
        if (age < igniteAt) return;  // hasn*t ignited yet
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

    static uint32_t randomRGB() {
        switch (rand() % 7) {
            case 0:
                return 0xFF0000;
            case 1:
                return 0x00FF00;
            case 2:
                return 0x0000FF;
            case 3:
                return 0xFFFF00;
            case 4:
                return 0x00FFFF;
            case 5:
                return 0xFF00FF;
            case 6:
            default:
                return 0xFFFFFF;
        }
    }

    void spawn(vector<fragment>& frags) {
        if (age < explodeAt) return;
        explodeAt = 9e9;  // large number: explode only once

        // === turn bright white with short duration ===
        prio = EXPLOSION;
        red = 0xFF;
        green = 0xFF;
        blue = 0xFF;
        age = 0.0f;
        duration = 0.3f;
        igniteAt = 0.0f;

        // === create new fragments ===
        const uint32_t nFrags = util::frand(3.0f, 5.0f) + 0.5f;
        float dv = 0;
        const float ddv = 0.5f * (1.0f - pos);  // explosion at low altitude ejects with high power
        const int rgb = randomRGB();
        for (uint32_t ixFrag = 0; ixFrag < nFrags; ++ixFrag) {
            dv += ddv;
            const float v0 = velocity + dv;
            const float duration = util::frand(0.8f, 1.1f);

            frags.emplace(frags.end(), EXPLSHELL, pos, v0, rgb, duration, /*explodeAt*/ 9e9, /*ignite at*/ 0.1);
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

    prio_e getPrio() { return prio; }

   protected:
    prio_e prio;
    float pos;
    float velocity;
    float duration;
    float age;
    float explodeAt;
    float igniteAt;
    float red;
    float green;
    float blue;
    uint32_t rgbOrig;
};
