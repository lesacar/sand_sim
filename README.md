# Particle simulation in C and raylib
Early development stages, currently supports 
- Opens on the monitor passed by -m N (monitor number) and spawns a window on that monitor, using that monitor's refresh rate
- 3 materials allow for various physics simulations
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