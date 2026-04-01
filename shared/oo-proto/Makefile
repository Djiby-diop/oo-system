CC := gcc
AR := ar
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude
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

LIB := $(BUILD_DIR)/liboo_proto.a
OBJ := $(BUILD_DIR)/oo_proto.o
TEST_BIN := $(BUILD_DIR)/test_oo_proto$(EXEEXT)

.PHONY: all test clean

all: $(LIB)

$(BUILD_DIR):
	$(MKDIR_BUILD)

$(OBJ): src/oo_proto.c include/oo_proto.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/oo_proto.c -o $@

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

$(TEST_BIN): tests/test_oo_proto.c $(OBJ) include/oo_proto.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_oo_proto.c $(OBJ) -o $@

test: $(TEST_BIN)
	$(TEST_BIN)

clean:
	$(CLEAN_CMD)