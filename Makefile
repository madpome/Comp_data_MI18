#Tu mets le nom de tes fichier .c
SOURCES = huffman.c huffmanDecod.c
OBJS = $(SOURCES:%.c=%.o)
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

clean:
	rm -f huffman huffmanDecod $(OBJS)
