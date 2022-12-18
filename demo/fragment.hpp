#pragma once
#include <cmath>

#include "pico/stdlib.h"
// fireworks simulator for programmable LED strip
class fragment {
   public:
    fragment(float pos, float velocity, uint32_t RGB, float duration) : pos(pos), velocity(velocity), duration(duration), age(0) {
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
        *(LEDs + posLED) = GRB; // controller end is bottom pos=0
        //  *(LEDs + nLEDs - 1 - posLED) = GRB; // reverse
    };

    void tick(float deltaTime, float accel) {
        velocity += accel * deltaTime;
        pos += velocity * deltaTime;
        age += deltaTime;
    };

    bool expired(){
        return pos < 0;
    }

   protected:
    float pos;
    float velocity;
    float duration;
    float age;
    float red;
    float green;
    float blue;
};