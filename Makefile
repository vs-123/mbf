OPTS=-Wall -Wextra -Wpedantic -std=c99 -Ivendors/dstr
SRC=$(filter-out %main.c %test.c, $(wildcard src/*.c))
BUILD=build
PROGRAM=mbf
BUILD_PROGRAM=$(BUILD)/$(PROGRAM)
TEST_PROGRAM=$(BUILD)/$(PROGRAM)_test

build: $(SRC)
	@ echo "[INFO] building..."
	@ mkdir -p $(BUILD)
	@ $(CC) -o $(BUILD_PROGRAM) $(OPTS) -O3 $(SRC) src/main.c
	@ echo "[SUCCESS] successfully built!"

b: build

test: $(SRC)
	@ echo "[INFO] building tests..."
	@ mkdir -p $(BUILD)
	@ $(CC) -o $(TEST_PROGRAM) $(OPTS) -O2 -g $(SRC) src/test.c
	@ echo "[INFO] running tests..."
	@ exec $(TEST_PROGRAM)

t: test

clean:
	@ echo "[INFO] cleaning..."
	@ rm -rf $(BUILD)
	@ echo "[SUCCESS] cleaned successfully!"

c: clean

.PHONY: build b test t clean c
