#include "setup.h"
#include <raylib.h>

int setup_stuff(int sc_wi, int sc_he, const char* WindowTitle, int log_lvl){
    SetTraceLogLevel(log_lvl);
    InitWindow(sc_wi,sc_he, WindowTitle);
    return 0;
}

int set_monitor_and_fps(int monitor){
    SetWindowMonitor(monitor-1);
    const int initial_fps = GetMonitorRefreshRate(monitor -1);
    SetTargetFPS(initial_fps);
    return 0;
}