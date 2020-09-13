@all:
	g++ `pkg-config --cflags gtk+-3.0` hello.c -o hello `pkg-config --libs gtk+-3.0`
