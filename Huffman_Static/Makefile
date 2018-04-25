SHELL := /bin/bash
SOURCES = encodeHuff.c decodeHuff.c
FILE = $(SOURCES) Makefile specHSMI
OBJS = $(SOURCES:%.c=%.o)
empty =
space = $(empty) $(empty)
bar = |
new = $(subst $(space),$(bar),$(FILE:%="%") *.txt)
CC = gcc
OPTS = -g -Wall
all : encodeHuff decodeHuff
% : %.o
	@echo "linking $(@:%.o=%) ..."
	@$(CC) $(OPTS) -o $@ $<

%.o : %.c
	@echo "building $(@:%=%) ..."
	@$(CC) $(OPTS) -c $<

encodeHuff : encodeHuff.o

decodeHuff : decodeHuff.o

encodeHuff.o : encodeHuff.c

decodeHuff.o : decodeHuff.c
#cleantest :
#	rm -f !("huffman.c"|"huffmanDecod.c"|"Makefile"|"specHSMI"|*.txt)
#cleanAll :
#	rm -f !($(new))
cleanO :
	rm -f $(OBJS)
clean:
	rm -f encodeHuff decodeHuff $(OBJS)
