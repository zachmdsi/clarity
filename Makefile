CC=gcc
CFLAGS=-Iinclude -Wall -Wextra
SRC=src/main.c src/concept.c
OUT=build/main.exe
all: $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)


run: all
	./$(OUT)

clean:
	rm -rf build
