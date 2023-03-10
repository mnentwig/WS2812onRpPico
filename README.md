# WS2812 LED control using Raspberry Pico

WS2812 is a digitally controllable RGB LED chip that is cheaply available, e.g. on flexible strips with 30, 60 or 144 LEDs per meter. LEDs run on 5 V but 3.3 V digital control usually works. 

A "reasonable" number of LEDs can be powered directly from the Pico board using +5V "VSys" from USB.

The "official" code sample for WS2812 makes use of the programmable state machines but they are a chip-specific, limited resource, internally complex and require pin planning. 

In comparison, the provided code supports an arbitrary number of LED chains (or just single LEDs) on any combination of GPIOs. The signal is simply bit-banged, knowing that one "for i ... write GPIO" iteration takes 4 clock cycles. As the GPIO port is hard-coded, the driver function needs to be replicated for each GPIO (152 bytes). It should be put into RAM, as a cache miss could cause bit errors.

The CPU is blocked during the duration of the transmission, which should not be an issue for the time in question (30 µs per LED plus 50 µs for the reset pulse). However, interrupts need to be disabled as they would cause bit errors.

# Building
Standard build process, expecting the sdk in /home/pi/pico/pico-sdk (hack warning: hard-coded against recommendations in CMakeFiles.txt. Edit or remove as needed)
``` 
cd WS2812onRpPico
mkdir build                             # create an empty folder "build"
cmake -S . -B build                     # set up cmake build files with "S"ource from current folder, "Build" into build folder
cmake --build build                     # run build process in build folder
cp build/WS2812.uf2 /media/pi/RPI-RP2   # hold button, attach to USB, then copy to board
```

# Demo application
See "demo" folder: Roman candle fireworks simulator :-)
- Adjust "nLEDs" to the length of the used strip (default: 30)
- Last line in "fragment.hpp: renderGRB()" can be uncommented to reverse direction
- math.m (Octave, documentation only) calculates the trajectory for a given time step (default: 10 ms / 100 Hz), the "true" continuous-time equivalent and its apex.