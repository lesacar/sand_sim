#include "arg_handler.h"
#include <raylib.h> // Assuming GetMonitorCount() is declared here

int handle_arguments(int argc, char *argv[]) {
    int current_monitor = 1;
    if (argc > 2) {
        if (strcmp(argv[1], "-m") == 0) { // Check if first argument is "-m"
            char *endptr;
            long num = strtol(argv[2], &endptr, 10); // Convert second argument to long

            if (errno == ERANGE || num < 1 || num > GetMonitorCount() || *endptr != '\0') {
                // Error handling for conversion failure or out-of-range value
                fprintf(stderr, "%s -m <N> (desired monitor number)\ndefault <N> is 1; Sets the desired monitor to spawn on\n", argv[0]);
                return -1;
            }
            current_monitor = (int)num;
        }
    }
    return current_monitor;
}

int ah_set_monitor(int monitor){
    return monitor--;
}