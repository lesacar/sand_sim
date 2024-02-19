# Sand simulation in C and raylib
Early development stages, currently supports 
- Opens on the monitor passed by -m N (monitor number) and spawns a window on that monitor, using that monitor's refresh rate
- dynamic raylib loading from ./dll/
 
# Linux
Shouldn't require any special packages, and raylib so's are bundled with the source code.
```
git clone https://github.com/lesacar/sand_sim
make
./build/main1
```