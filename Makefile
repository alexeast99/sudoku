# --cflags causes pkg-config to output a list of include directories
# --libs requests lists of libraries for compiler to link with

# target ... : prerequisites ...
#			recipe
#			...
#			...

# -c 	  --> Don't run linked. Output is object files. 
# -o file --> Place output in file

shell   = /bin/sh
objects = main.o board.o
cc	= g++
src 	= src
inc	= include
include	= $(inc) $(src)

sudoku: $(OBJECTS)
	$(cc) -o sudoku $(OBJECTS) `pkg-config gtkmm-3.0 --cflags --libs` -I $(include)

main.o: $(src)/main.cpp $(inc)/board.h
	$(cc) -c $(src)/main.cpp `pkg-config gtkmm-3.0 --cflags --libs` -I $(include)

#board.o: $(src)/board.cpp $(inc)/board.h
#	$(cc) -c board.cpp -I $(include)

.PHONY: clean
clean:
	-rm *.o sudoku
