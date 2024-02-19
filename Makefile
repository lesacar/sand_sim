CC = gcc
CFLAGS = -std=c99 -O3
RM = rm
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

LDFLAGS = -lraylib -lm
INCLUDE = -I./src/include -L./static
BUILD_DIR = build
TARGET = $(BUILD_DIR)/main
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR = dbg
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(INCLUDE) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

check: $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) -I$(INCLUDE) $(WARN) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS)
	cppcheck -q -I../raylib/include/ --enable=warning $(SRC)

clean:
	$(RM) -rf $(BUILD_DIR) $(OBJ_DIR)
