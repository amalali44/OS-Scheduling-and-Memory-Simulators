# ID BOX:
# Name: Amal Ali
# Assignment #3
# Instructor’s Name: Salwa Abougamila
# File purpose: To compile several C source codes with one command

# Variables/ Macros:
CC = gcc 
CFLAGS= -std=c11 -Wall -Wextra -pedantic 
PACKAGE_NAME = ali_cmpt_360_A3.zip

#Target 1: vmsim.c
all: vmsim.c
	$(CC) $(CFLAGS) vmsim.c -o vmsim

vmsim.o: vmsim.c vmsim.h
	$(CC) $(CFLAGS) -c vmsim.c


clean:
	 rm *.o 

#valgrind: Runs the program with valgrind to check for memory leaks
valgrind: vmsim
	valgrind --leak-check=full ./vmsim


#Packagae: Condenses all the specified files into a zip file
package:
	zip $(PACKAGE_NAME) vmsim.c vmsim.h makefile README.md tests

