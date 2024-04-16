#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include <stdint.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Function prototype
void version_info(char** argv, int argc);
int32_t handle_arguments(int32_t argc, char *argv[]);
int32_t ah_set_monitor(int32_t monitor);

#endif /* ARG_HANDLER_H */
