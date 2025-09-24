# ID BOX:
# Name: Amal Ali
# Assignment #1
# Instructor’s Name: Salwa Abougamila
# File purpose: To compile several C source codes with one command

# Variables/ Macros:
CC = gcc 
CFLAGS= -Wall -std=c99
PACKAGE_NAME = ali_cmpt_360_A1.zip

#Target 1: userclean.c
userclean: userclean.c
	$(CC) $(CFLAGS) userclean.c -o userclean

#Target 2: Testing
testuserclean: userclean
	./userclean < input.txt > usercleanOut.txt 
	diff usercleanOut.txt expected.txt

#Clean: Cleans out all the executable files 
clean:
	 rm *.o 

#Packagae: Condenses all the specified files into a zip file
package:
	zip -czvf $(PACKAGE_NAME) userclean.c makefile input.txt expected.txt usercleanOut.txt README.txt