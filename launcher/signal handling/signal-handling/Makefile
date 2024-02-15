CC = gcc
LD = gcc
GPERF = gperf
CXX = g++
DEPS = lib.h
SRCS = server-bad.c server-good.c server-best.c lib.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = lib.o server-bad.o server-good.o server-best.o
LIBS = -lm -lrt
DEBUG = -g -Wall
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =
GPERFFLAGS = --language=ANSI-C

TARGET  = server-bad
TARGET1 = server-good
TARGET2 = server-best

RM = rm -f

all: release

server-bad:    server-bad.o lib.o
server-good:   server-good.o lib.o
server-best:   server-best.o lib.o

server-bad.o:  server-bad.c
server-good.o: server-good.c
server-best.o: server-best.c
lib.o:         lib.c

release: server-bad.c server-good.c server-best.c $(DEPS)
	$(CC) -Wall -pthread -c $(CFLAGS) server-bad.c
	$(CC) -Wall -pthread -c $(CFLAGS) server-good.c
	$(CC) -Wall -pthread -c $(CFLAGS) server-best.c
	$(CC) -Wall -pthread -c $(CFLAGS) lib.c
	$(CC) -Wall -pthread -o $(TARGET) $(CFLAGS) server-bad.c lib.o $(LIBS)
	$(CC) -Wall -pthread -o $(TARGET1) $(CFLAGS) server-good.c lib.o $(LIBS)
	$(CC) -Wall -pthread -o $(TARGET2) $(CFLAGS) server-best.c lib.o $(LIBS)

clean:
	$(RM) $(OBJS) $(TARGET) $(TARGET1) $(TARGET2)

distclean: clean

.PHONY: clean distclean all debug release
