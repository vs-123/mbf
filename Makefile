OPTS=-Wall -Wextra -Wpedantic -std=c11 -g
SRC=$(wildcard src/*.c)
BUILD=build
PROGRAM=mbf
BUILD_PROGRAM=$(BUILD)/$(PROGRAM)

build: $(SRC)
	@ echo "[INFO] building..."
	@ mkdir -p $(BUILD)
	@ $(CC) -o $(BUILD_PROGRAM) $(OPTS) $(SRC)

run: build
	@ echo "[INFO] running..."
	@ exec $(BUILD_PROGRAM)

clean:
	@ echo "[INFO] cleaning..."
	@ rm -rf $(BUILD)

.PHONY: build run clean
