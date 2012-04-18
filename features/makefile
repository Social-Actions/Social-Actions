#
# Makefile for Twitter - Trust Project
#

TARGET = conv prop
OBJS = conversation_trust.o propagation_trust.o
CC = g++
CFLAGS = -g -Wall
INCLUDE = -I ../util

all: $(TARGET)

conv: conversation_trust.o
	$(CC) $(CFLAGS) conversation_trust.o -o $@

prop: propagation_trust.o
	$(CC) $(CFLAGS) propagation_trust.o -o $@

clean:
	$(RM) $(OBJS) $(TARGET)

.cc.o:
	$(CC) $(INCLUDE) $(CFLAGS) $< -c 