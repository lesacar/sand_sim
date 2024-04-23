#include "arg_handler.h"
#include "setup.h"
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

void version_info(char** argv, int argc) {
	if (argc != 2) { return; }
	if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
	{
#ifdef WINREL 
		const char* build_type = "Windows Release";
#elif WINDBG
		const char* build_type = "Windows Debug";
#elif UDEBUG 
		const char* build_type = "Unix Debug";
#elif URELEASE 
		const char* build_type = "Unix Release";
#elif WINREL 
		const char* build_type = "Windows Release";
#else 
		const char* build_type = "Unix DON'T USE";
#endif
		printf("%s\n\tBUILD TYPE: %s\n\tBlock size: %d\n", basename(argv[0]), build_type, BLOCK_SIZE);
		printf("\tWindow size: %dx%d\n\tGrid size: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT, COLS, ROWS);
		printf("\tMAX threads: %d\n", NUM_THREADS);
		printf("\tMAX cfg file size: %d\n", MAX_CFG_SIZE);
		printf("\tMAX cfg file lines: %d\n", MAX_CFG_LINE);
		printf("\tMAX cfg file lines: %d\n", MAX_CFG_LINE);
		printf("\tUnique materials: %d:\n", MatCount);
		for (int i = 0; i < MatCount; ++i) {
			if (i > 0 && i % 4 == 0) { printf("\n"); }
			printf("\t%s", str_mat(i));
		}
		 printf("\n");
		exit(EXIT_SUCCESS);
	}
}

int32_t ah_set_monitor(int32_t monitor){
    return monitor--;
}
