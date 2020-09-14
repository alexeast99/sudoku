@all:
	g++ `pkg-config --cflags gtk+-3.0` src/sudoku.cpp -o sudoku `pkg-config --libs gtk+-3.0`
