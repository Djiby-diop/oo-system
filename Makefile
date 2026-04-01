CC := gcc
AR := ar
EXEEXT ?=

ifeq ($(OS),Windows_NT)
EXEEXT := .exe
endif

.PHONY: all proto cli test clean

all: proto cli

proto:
	$(MAKE) -C shared/oo-proto CC="$(CC)" AR="$(AR)" EXEEXT="$(EXEEXT)"

cli: proto
	$(MAKE) -C interface/cli CC="$(CC)" EXEEXT="$(EXEEXT)"

test: proto cli
	$(MAKE) -C shared/oo-proto test CC="$(CC)" AR="$(AR)" EXEEXT="$(EXEEXT)"
	$(MAKE) -C interface/cli test CC="$(CC)" EXEEXT="$(EXEEXT)"

clean:
	$(MAKE) -C shared/oo-proto clean EXEEXT="$(EXEEXT)"
	$(MAKE) -C interface/cli clean EXEEXT="$(EXEEXT)"