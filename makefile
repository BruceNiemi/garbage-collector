CXX = clang
CPPFLAGS = -g

.PHONY: clean test

all: tests

tests: tests.o ObjectManager.o

clean:
	rm -f tests.o ObjectManager.o
