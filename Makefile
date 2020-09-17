# --cflags causes pkg-config to output a list of include directories
# --libs requests lists of libraries for compiler to link with

@all:
	g++ src/sudoku.cpp -o sudoku `pkg-config gtkmm-3.0 --cflags --libs`
