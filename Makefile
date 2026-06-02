CC = clang
CFLAGS = -Wall -Wextra -std=c11

SRC = main.c sticker.c storage.c sc_convert.c
OUT = stracker 

all:
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
