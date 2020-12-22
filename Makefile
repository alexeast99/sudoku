# Last Updated: 09/19/2020
# Author: Alex Eastman
# Contact: alexeast@buffalo.edu


# --cflags causes pkg-config to output a list of include directories
# --libs requests lists of libraries for compiler to link with

# target ... : prerequisites ...
#			recipe
#			...
#			...

# -c 	  --> Don't run linker. Output is object files.
# -o file --> Place output in file

shell	= /bin/sh
cc		= g++
src 	= src
inc		= inc
obj		= objs
gtkflags  = `pkg-config gtkmm-3.0 --cflags --libs` -Wall -Werror
glibflags = `pkg-config glibmm-2.4 --cflags --libs` -Wall -Werror
objects = $(obj)/main.o $(obj)/board.o

sudoku: $(objects)
	$(cc) -o $@ $(objects) $(gtkflags) -I$(inc)

$(obj)/main.o: $(src)/main.cpp $(inc)/board.h
	$(cc) -c $(src)/main.cpp -o $@ $(gtkflags) -I$(inc)

$(obj)/board.o: $(src)/board.cpp $(inc)/board.h
	$(cc) -c $(src)/board.cpp -o $@ $(glibflags) -I$(inc)


.PHONY: clean
clean:
	-rm objs/* sudoku
