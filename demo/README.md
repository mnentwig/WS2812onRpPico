# WS2812 LED control on Raspberry Pico demo

Roman candle fireworks simulator with a few exploding shells thrown into the mix :-)
- Adjust "nLEDs" to the length of the used strip (default: 30)
- Last line in "fragment.hpp: renderGRB()" can be uncommented to reverse direction
- math.m (Octave, documentation only) calculates the trajectory for a given time step (default: 10 ms / 100 Hz), the "true" continuous-time equivalent and its apex.

Note: The demo works fine with on-board power from a 500 mA USB charger as only a few LEDs are active at a time. General use of the LED strip e.g. for illumination purposes would require a higher-rated power supply.