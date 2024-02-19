#include "arg_handler.h"
#include <raylib.h> // Assuming GetMonitorCount() is declared here


int32_t handle_arguments(int32_t argc, char *argv[]) {
    int32_t current_monitor = 1;
    if (argc > 2) {
        if (strcmp(argv[1], "-m") == 0) { // Check if first argument is "-m"
            char *endptr;
            long num = strtol(argv[2], &endptr, 10); // Convert second argument to long

            if (errno == ERANGE || num < 1 || num > GetMonitorCount() || *endptr != '\0') {
                // Error handling for conversion failure or out-of-range value
                fprintf(stderr, "%s -m <N> (desired monitor number)\ndefault <N> is 1; Sets the desired monitor to spawn on\n", argv[0]);
                return -1;
            }
            current_monitor = (int32_t)num;
        }
    }
    return current_monitor;
}

int32_t ah_set_monitor(int32_t monitor){
    return monitor--;
}