#
# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = g++
CFLAGS  = -g -std=c++14

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: oc

# To create the executable file oc we need the object files
# countwords.o, counter.o, and scanner.o:
#
oc:  auxlib.o cppstrtok.o string_set.o
	$(CC) $(CFLAGS) -o oc auxlib.o cppstrtok.o string_set.o

# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
auxlib.o:  auxlib.cpp  
	$(CC) $(CFLAGS) -c auxlib.cpp

# To create the object file counter.o, we need the source files
# counter.c and counter.h:
#
cppstrtok.o: cppstrtok.cpp 
	$(CC) $(CFLAGS) -c cppstrtok.cpp

# To create the object file scanner.o, we need the source files
# scanner.c and scanner.h:
#
string_set.o:  string_set.cpp
	$(CC) $(CFLAGS) -c string_set.cpp

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) *.o *~

spotless: 
	$(RM) oc *.o *~
