SHELL := /bin/bash
SOURCES = huffman.c huffmanDecod.c
FILE = $(SOURCES) Makefile specHSMI
OBJS = $(SOURCES:%.c=%.o)
empty =
space = $(empty) $(empty)
bar = |
new = $(subst $(space),$(bar),$(FILE:%="%") *.txt)
CC = gcc
OPTS = -g -Wall
all : huffman huffmanDecod
% : %.o
	@echo "linking $(@:%.o=%) ..."
	@$(CC) $(OPTS) -o $@ $<

%.o : %.c
	@echo "building $(@:%=%) ..."
	@$(CC) $(OPTS) -c $<

huffman : huffman.o

huffmanDecod : huffmanDecod.o

huffman.o : huffman.c

huffmanDecod.o : huffman.c
#cleantest :
#	rm -f !("huffman.c"|"huffmanDecod.c"|"Makefile"|"specHSMI"|*.txt)
#cleanAll :
#	rm -f !($(new))
cleanO :
	rm -f $(OBJS)
clean:
	rm -f huffman huffmanDecod $(OBJS)
