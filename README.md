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
mkdir build
cmake -S . -B build
```
