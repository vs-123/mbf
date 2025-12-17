CC=cc
OPTS=-Wall -Wextra -Wpedantic -std=c11 -g
SRC=$(filter-out %main.c %test.c, $(wildcard src/*.c))
BUILD=build
PROGRAM=mbf
BUILD_PROGRAM=$(BUILD)/$(PROGRAM)
TEST_PROGRAM=$(BUILD)/$(PROGRAM)_test

build: $(SRC)
	@ echo "[INFO] building..."
	@ mkdir -p $(BUILD)
	@ $(CC) -o $(BUILD_PROGRAM) $(OPTS) $(SRC) src/main.c

run: build
	@ echo "[INFO] running..."
	@ exec $(BUILD_PROGRAM)

test: $(SRC)
	@ echo "[INFO] building tests..."
	@ mkdir -p $(BUILD)
	@ $(CC) -o $(TEST_PROGRAM) $(OPTS) $(SRC) src/test.c
	@ echo "[INFO] running tests..."
	@ exec $(TEST_PROGRAM)

clean:
	@ echo "[INFO] cleaning..."
	@ rm -rf $(BUILD)

.PHONY: build run clean
