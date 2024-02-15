CC = gcc
LD = gcc
GPERF = gperf
CXX = g++
DEPS = lib.h
SRCS = server-best.c lib.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = lib.o server-best.o
LIBS = -lm -lrt
DEBUG = -g -Wall
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =
GPERFFLAGS = --language=ANSI-C

TARGET = server-best

RM = rm -f

all: release

server-best:   server-best.o lib.o

server-best.o: server-best.c
lib.o:         lib.c

release: server-best.c $(DEPS)
	$(CC) -Wall -pthread -c $(CFLAGS) server-best.c
	$(CC) -Wall -pthread -c $(CFLAGS) lib.c
	$(CC) -Wall -pthread -o $(TARGET) $(CFLAGS) server-best.c lib.o $(LIBS)

clean:
	$(RM) $(OBJS) $(TARGET)

distclean: clean

.PHONY: clean distclean all debug release
