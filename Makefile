CC = gcc
CFLAGS = -std=c99 -O3 -flto
WARN = -Wall -Wextra -Wpedantic \
         -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition \
         -Wimplicit-function-declaration -Wreturn-type \
         -Wuninitialized -Wunused-result -Wunused-variable \
         -Wunused-value -Wunused-parameter \
         -Wformat-security -Warray-bounds \
         -Wmaybe-uninitialized -Wfloat-equal \
         -Wpointer-arith -Wunused-but-set-variable \
         -Wunused-function -Wshadow \
         -Wduplicated-cond -Wrestrict \
         -Wlogical-op -Wnull-dereference -Wdouble-promotion \
         -Wformat=2 -Wimplicit-int -Wmissing-include-dirs \
         -Wundef -Wpointer-sign \
         -Wwrite-strings -Waddress \
         -Wlogical-op -Waggregate-return \
         -Wconversion -Wsign-compare \
         -Wtype-limits

LDFLAGS = -lm -lraylib
INCLUDE = ../raylib/include
RAYLIB_A = ../raylib
TARGET = main
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR = dbg
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) -I$(INCLUDE) -L$(RAYLIB_A) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

check: $(SRC)
	$(CC) -I$(INCLUDE) -L$(RAYLIB_A) $(WARN) $(CFLAGS) $(SRC) -o main $(LDFLAGS)
	cppcheck -q -I../raylib/include/ --enable=warning $(SRC)

clean:
	$(RM) $(TARGET) $(OBJ)
