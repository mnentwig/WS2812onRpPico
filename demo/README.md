# WS2812 LED control on Raspberry Pico demo

Roman candle fireworks simulator with a few exploding shells thrown into the mix :-)
- Adjust "nLEDs" to the length of the used strip (default: 30)
- Last line in "fragment.hpp: renderGRB()" can be uncommented to reverse direction
- math.m (Octave, documentation only) calculates the trajectory for a given time step (default: 10 ms / 100 Hz), the "true" continuous-time equivalent and its apex.