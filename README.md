![bloom](https://github.com/lesacar/sand_sim/assets/69954403/5f8338fd-d150-450a-9fc1-1dc35bd2c6a8)
*A sand_sim (physim) simulation running with bloom enabled*
# Particle simulation in C and raylib
*linux raylib is in ./static ; windows raylib is in ./static/windows*
***The binary must be ran from the root git directory as ./build/physim, otherwise resources will not load***
### Controls are in controls.txt
- Press SPACE to pause
- Press 's' to toggle bloom shader on or off
Early development stages, currently supports 
- config file **"sim.cfg"** which should be a directory above physim(.exe)\
 |-build\
 |    |\
 |    |---physim(.exe)\
 |\
 |-sim.cfg

### 4 materials allow for various physics simulations
- Sand; falls into a pyramid
- Stone; stationary, not affected by anything
- Water; similar to sand but also spreads horizontally
- Steam; floats upwards and through water, not yet im
plemented floating through solid objects 

### Config file currently has 
- fps=60
- brush_size=20
- brush_mode=true
There is an example config in src/
The brush_size is the size of the circle when placing tiles, brush mode is whether you want a full circle, or random ms-paint brush-like placement.

![physim](https://github.com/lesacar/sand_sim/assets/69954403/51cfbcb8-542a-4df6-81f0-1e4b2af80afe)

### UI explained - top left
- tiles on screen: how many non-empty tiles are currently on the screen, including behind the ui 
- Yellow text: small hint messages, some recent hints are missing 
- colored rectangles: this is where you pick the material to place, there is 4, steam is next to the 3rd one (stone) but I didn't add an icon yet. There is no indication if you actually selected a material, just click it a few times to make sure
### UI explained - top right 
- UPS on #2: updates per second on the 2nd processor thread, the one that is doing updates in the background, it tries to be 60 all the time, but will slow down if you have a slower CPU 
- mx, my, i, j: mouse coordinates on the window and the corresponding tile position (tracks the tile beneath your mouse) 
- **mini debug display**, info of the currently hovered-over tile 
- M: material, 0 = empty, 1 = Sand, 2 = Water, 3 = Stone, 4 = Steam 
- C: rgb color value 
- FR: friction
- vX and vY: horizontal and vertical velocity 
- FALL: is the current tile freefalling or not 
- SF: spreadFactor, used in liquids or liquid-likes
- MASS: mass


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

