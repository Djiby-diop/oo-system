CC := gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -I../../shared/oo-proto/include
BUILD_DIR ?= build
EXEEXT ?=

ifeq ($(OS),Windows_NT)
EXEEXT := .exe
MKDIR_BUILD := cmd /c if not exist build mkdir build
CLEAN_CMD := cmd /c if exist build rmdir /s /q build
else
MKDIR_BUILD := mkdir -p build
CLEAN_CMD := rm -rf build
endif

PROTO_DIR := ../../shared/oo-proto
PROTO_LIB := $(PROTO_DIR)/build/liboo_proto.a
BIN := $(BUILD_DIR)/oo$(EXEEXT)

.PHONY: all test clean

all: $(BIN)

$(BUILD_DIR):
	$(MKDIR_BUILD)

$(BIN): src/oo_cli.c $(PROTO_LIB) ../../shared/oo-proto/include/oo_proto.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) src/oo_cli.c $(PROTO_LIB) -o $@

test: $(BIN)
	$(BIN) help

clean:
	$(CLEAN_CMD)