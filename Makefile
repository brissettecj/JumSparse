BUILD_DIR  := build
CMAKE_ARGS := -DBUILD_TESTS=ON
JOBS       := $(shell nproc 2>/dev/null || echo 4)

.PHONY: all configure build test clean rebuild

## Default: configure + build + test
all: build

## Create build directory and run CMake configure
configure:
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. $(CMAKE_ARGS)

## Build the project (auto-configures if needed)
build: configure
	cmake --build $(BUILD_DIR) -j$(JOBS)

## Run all tests (auto-builds if needed)
test: build
	cd $(BUILD_DIR) && ctest --output-on-failure -j$(JOBS)

## Remove the build directory entirely
clean:
	rm -rf $(BUILD_DIR)

## Clean + full rebuild
rebuild: clean build
