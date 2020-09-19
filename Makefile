# --cflags causes pkg-config to output a list of include directories
# --libs requests lists of libraries for compiler to link with

# target ... : prerequisites ...
#			recipe
#			...
#			...

# -c 	  --> Don't run linker. Output is object files. 
# -o file --> Place output in file

shell   = /bin/sh
cc		= g++
src 	= src
inc		= inc
obj		= objs
cflags  = `pkg-config gtkmm-3.0 --cflags --libs`
objects = $(obj)/main.o $(obj)/board.o

sudoku: $(objects)
	$(cc) -o $@ $(objects) $(cflags) -I$(inc)

$(obj)/main.o: $(src)/main.cpp $(inc)/board.h
	$(cc) -c $(src)/main.cpp -o $@ $(cflags) -I$(inc)

$(obj)/board.o: $(src)/board.cpp $(inc)/board.h
	$(cc) -c $(src)/board.cpp -o $@ -I$(inc)

.PHONY: clean
clean:
	-rm objs/* sudoku
