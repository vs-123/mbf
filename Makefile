OPTS=-Wall -Wextra -Wpedantic
SRC=$(wildcard src/*.c)
BUILD=build/
PROGRAM=mbf
BUILD_PROGRAM=$(BUILD)/$(PROGRAM)

build: $(SRC)
	@ echo "[INFO] building..."
	@ mkdir -p $(BUILD_PROGRAM)
	@ $(CC) -o $(BUILD_PROGRAM) $(OPTS) $(SRC)

run: build
	@ echo "[INFO] running..."
	@ exec $(BUILD_PROGRAM)

clean: $(BUILD)
	@ echo "[INFO] cleaning..."
	@ rm -rf $(BUILD)

.PHONY: build run clean
