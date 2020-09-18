# --cflags causes pkg-config to output a list of include directories
# --libs requests lists of libraries for compiler to link with

# target ... : prerequisites ...
#			recipe
#			...
#			...

shell   = /bin/sh
objects = main.o board.o
cc			= gcc
src 		= src
include = include
other 	= $(include) $(src)

sudoku: $(OBJECTS)
	$(cc) -o sudoku $(OBJECTS) `pkg-config gtkmm-3.0 --cflags --libs`

main.o: $(src)/main.cpp $(include)/board.h
	$(cc) main.cpp `pkg-config gtkmm-3.0 --cflags --libs` -I $(other)

board.o: $(src)/board.cpp $(include)/board.h
	$(cc) board.cpp -I $(other)

.PHONY: clean
clean:
	-rm *.o sudoku
