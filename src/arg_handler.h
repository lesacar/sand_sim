#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Function prototype
int handle_arguments(int argc, char *argv[]);
int ah_set_monitor(int monitor);

#endif /* ARG_HANDLER_H */
