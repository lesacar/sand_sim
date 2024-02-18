#ifndef SETUP_H
#define SETUP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Function prototype
int setup_stuff(int sc_wi, int sc_he, const char* WindowTitle, int log_lvl);
int set_monitor_and_fps(int monitor);

#endif /* SETUP_H */
