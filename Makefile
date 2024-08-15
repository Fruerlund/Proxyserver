CC = gcc # Flag for implicit rules
CFLAGS = -g # Flag for implicit rules. Turn on debug info
	
src_files = $(wildcard /src/*.c)


all:
	$(CC) -pthread $(wildcard src/*.c) -o build/socks
clean:
	echo "Clean"
	rm build/*
