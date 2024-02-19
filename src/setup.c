#include "setup.h"
#include <raylib.h>

int32_t setup_stuff(int32_t sc_wi, int32_t sc_he, const char* WindowTitle, int32_t log_lvl){
    SetTraceLogLevel(log_lvl);
    InitWindow(sc_wi,sc_he, WindowTitle);
    return 0;
}

int32_t set_monitor_and_fps(int32_t monitor){
    SetWindowMonitor(monitor-1);
    const int32_t initial_fps = GetMonitorRefreshRate(monitor -1);
    SetTargetFPS(initial_fps);
    return 0;
}