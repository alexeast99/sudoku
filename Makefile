@all:
	g++ src/sudoku.cpp -o sudoku `pkg-config gtkmm-3.0 --cflags --libs` 
