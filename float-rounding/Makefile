.SUFFIXES:
MAKEFLAGS += -r
SHELL := /bin/bash

GCC = g++
CLANG = clang++-3.5

.PHONY: all list clean

TARGETS = float-rounding.gcc.03 float-rounding.gcc.03.no-math-errno float-rounding.gcc.03.fast-math \
	float-rounding.clang.03 float-rounding.clang.03.no-math-errno float-rounding.clang.03.fast-math

all: ${TARGETS}

list:
	@echo "TARGETS=${TARGETS}"

clean:
	rm -rf ${TARGETS}

float-rounding.gcc.03: float-rounding.cpp
	${GCC} -std=c++11 -Wall -Wextra -pedantic -O3 $^ -o $@

float-rounding.gcc.03.no-math-errno: float-rounding.cpp
	${GCC} -std=c++11 -Wall -Wextra -pedantic -O3 -fno-math-errno $^ -o $@

float-rounding.gcc.03.fast-math: float-rounding.cpp
	${GCC} -std=c++11 -Wall -Wextra -pedantic -O3 -ffast-math $^ -o $@

float-rounding.clang.03: float-rounding.cpp
	${CLANG} -std=c++11 -Wall -Wextra -pedantic -O3 -mllvm -inline-threshold=10000 $^ -o $@

float-rounding.clang.03.no-math-errno: float-rounding.cpp
	${CLANG} -std=c++11 -Wall -Wextra -pedantic -O3 -mllvm -inline-threshold=10000 -fno-math-errno $^ -o $@

float-rounding.clang.03.fast-math: float-rounding.cpp
	${CLANG} -std=c++11 -Wall -Wextra -pedantic -O3 -mllvm -inline-threshold=10000 -ffast-math $^ -o $@
