#=========================================================================
# Simple Makefile for launcher
#
# Author:      David Witten, KD0EAG
# Date:        January 3, 2023
# License:     GPL 3.0
# Note:        
#=========================================================================
CC = gcc
LD = gcc
GPERF = gperf
CXX = g++
DEPS = launcher.h cmdmgr.h
SRCS = launcher.c cmdmgr.c testchild.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = launcher.o cmdmgr.o testchild.o
LIBS = -lm -lpigpio -lrt
LIBS2 = -lm -lpigpiod_if2 -lrt
DEBUG = -g -Wall
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =
GPERFFLAGS = --language=ANSI-C 

TARGET = launcher
TARGET2 = testchild

RM = rm -f

all: release

debug: launcher.c testchild.c $(DEPS) 
	$(CC) -c $(DEBUG) launcher.c
	$(CC) -c $(DEBUG) cmdmgr.c
	$(CC) -c $(CFLAGS) testchild.c
	$(CC) -o $(TARGET) $(DEBUG) launcher.c cmdmgr.o $(LIBS)
	$(CC) -o $(TARGET2) $(DEBUG) testchild.c $(LIBS2)

release: launcher.c testchild.c $(DEPS)
	$(CC) -Wall -pthread -c $(CFLAGS) launcher.c
	$(CC) -Wall -pthread -c $(CFLAGS) cmdmgr.c
	$(CC) -Wall -pthread -c $(CFLAGS) testchild.c
	$(CC) -Wall -pthread -o $(TARGET) $(CFLAGS) launcher.c cmdmgr.o $(LIBS)
	$(CC) -Wall -pthread -o $(TARGET2) $(CFLAGS) testchild.c $(LIBS2)

clean:
	$(RM) $(OBJS) $(TARGET) $(TARGET2)

distclean: clean
	
.PHONY: clean distclean all debug release

