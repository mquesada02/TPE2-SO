CC = gcc
CFLAGS = -Wall -g

all: smmtest

smm.o:
	$(CC) $(CFLAGS) -c -o smm.o SimpleMemoryManagerADT.c
smmtest.o:
	$(CC) $(CFLAGS) -c -o smmtest.o test/SimpleMemoryManagerTest.c
smmtest: smm.o smmtest.o
	$(CC) $(CFLAGS) -o bin/SimpleMemoryManagerTest smm.o smmtest.o
	make cleanObjects
cleanObjects:
	rm -f smmtest.o
	rm -f smm.o 
clean:
	make cleanObjects
	rm -f bin/SimpleMemoryManagerTest
	

