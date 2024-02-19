#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Function prototype
int32_t handle_arguments(int32_t argc, char *argv[]);
int32_t ah_set_monitor(int32_t monitor);

#endif /* ARG_HANDLER_H */
