# Particle simulation in C and raylib
![physim](https://github.com/lesacar/sand_sim/assets/69954403/51cfbcb8-542a-4df6-81f0-1e4b2af80afe)
*linux raylib is in ./static ; windows raylib is in ./static/windows*
Early development stages, currently supports 
- config file **"sim.cfg"** which should be a directory above physim(.exe)
 |-build
 |    |
 |    |---physim(.exe)
 |
 |-sim.cfg

- 3 materials allow for various physics simulations
- Sand; falls into a pyramid
- Stone; stationary, not affected by anything
- Water; similar to sand but also spreads horizontally
- Steam; floats upwards and through water, not yet im
plemented floating through solid objects 

# Linux target
Requires the usual build tools like ```make``` and ```gcc``` in PATH, raylib is provided in the static directory, you can also put your custom version of raylib in the static directory
```
git clone https://github.com/lesacar/sand_sim
make
./build/physim
```

# Windows target
Requires ```make``` and ```x86_64-w64-mingw32-gcc``` in your path, you should also modify the Makefile for windows paths delimitires ```\\``` instead of *nix ```/```
```make win``` runs perfectly fine if building from Linux, so **maybe wsl could** work 

