gcc -g -I./src/include -Wall -Wextra -std=c99 -fopenmp -c src/arg_handler.c -o dbg/arg_handler.o -L./static -lraylib -lm
gcc -g -I./src/include -Wall -Wextra -std=c99 -fopenmp -c src/main.c -o dbg/main.o -L./static -lraylib -lm
gcc -g -I./src/include -Wall -Wextra -std=c99 -fopenmp -c src/setup.c -o dbg/setup.o -L./static -lraylib -lm
gcc -g -I./src/include -Wall -Wextra -std=c99 -fopenmp dbg/arg_handler.o dbg/main.o dbg/setup.o -o build/sand -L./static -lraylib -lm
