#ifndef SETUP_H
#define SETUP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

// Function prototype
int32_t setup_stuff(int32_t sc_wi, int32_t sc_he, const char* WindowTitle, int32_t log_lvl, bool fullscreen);
int32_t set_monitor_and_fps(int32_t monitor);

#endif /* SETUP_H */
