# Sand simulation in C and raylib
Early development stages, currently supports 
- Opens on the monitor passed by -m N (monitor number) and spawns a window on that monitor, using that monitor's refresh rate
- dynamic raylib loading from ./dll/
 
# Linux
Requires raylib installed system wide or libraylib.a in the static directory
```
git clone https://github.com/lesacar/sand_sim
make
./build/main1
```