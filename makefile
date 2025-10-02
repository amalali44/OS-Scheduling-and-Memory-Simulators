# ID BOX:
# Name: Amal Ali
# Assignment #1
# Instructor’s Name: Salwa Abougamila
# File purpose: To compile several C source codes with one command

# Variables/ Macros:
CC = gcc 
CFLAGS= -std=c11 -Wall -Wextra -pedantic 
PACKAGE_NAME = ali_cmpt_360_A2.zip

#Target 1: sched.c
sched: sched.c
	$(CC) $(CFLAGS) sched.c -o sched

sched.o: sched.c sched.h
	$(CC) $(CFLAGS) -c sched.c


clean:
	 rm *.o 

#valgrind: Runs the program with valgrind to check for memory leaks
valgrind: sched
	valgrind --leak-check=full ./sched



#Packagae: Condenses all the specified files into a zip file
package:
	zip $(PACKAGE_NAME) sched.c sched.h makefile README.txt

