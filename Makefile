CROSS =
CC = $(CROSS)gcc
AR = $(CROSS)ar
RANLIB = $(CROSS)ranlib
LUA = lua
LUA_LIBS = $(shell pkg-config --libs $(LUA))
LUA_CFLAGS = $(shell pkg-config --cflags $(LUA))
SO_SFX = so

.PHONY: all clean
.SUFFIXES:

all: lecho/core.a lecho/core.$(SO_SFX)

lecho/core.o: lecho.c
	mkdir -p lecho
	$(CC) -Wall -Wextra -fPIC $(LUA_CFLAGS) -c lecho.c -o lecho/core.o

lecho/core.a: lecho/core.o
	$(AR) rcs lecho/core.a lecho/core.o
	$(RANLIB) lecho/core.a

lecho/core.$(SO_SFX): lecho/core.o
	$(CC) -shared $(LUA_LIBS) -o lecho/core.$(SO_SFX) lecho/core.o

clean:
	rm -f lecho/core.a lecho/core.$(SO_SFX) lecho/core.o
	rm -f lecho/core.dll lecho/core.so
	rmdir lecho
