# Particle simulation in C and raylib
*linux raylib is in ./static ; windows raylib is in ./static/windows*
Early development stages, currently supports 
- Opens on the monitor passed by ./sand -m N (monitor number) and spawns a window on that monitor, using that monitor's refresh rate
**By default opens on the primary monitor if no -m is passed**
- 3 materials allow for various physics simulations
- Sand; falls into a pyramid
- Stone; stationary, not affected by anything
- Water; similar to sand but also spreads horizontally

Water is currently broken, sand uses a "for loop biased" algorithm, I will fix the sand algorithm after fixing water, because the sand for loop bias is barely noticeable
 
# Linux target
Requires the usual build tools like ```make``` and ```gcc``` in PATH, raylib is provided in the static directory, you can also put your custom version of raylib in the static directory
```
git clone https://github.com/lesacar/sand_sim
make
./build/main1
```

# Windows target
Requires ```make``` and ```x86_64-w64-mingw32-gcc``` in your path, you should also modify the Makefile for windows paths delimitires ```\\``` instead of *nix ```/```
```make win``` runs perfectly fine if building from Linux, so **maybe wsl could** also work