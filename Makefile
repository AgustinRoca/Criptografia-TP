CFLAGS = -Wall -Wextra -pedantic -g -std=c11 -fsanitize=address -Wno-unused-parameter

MAIN_OBJ = main.o distribuir.o  recuperar.o utils.o

all: main

main: $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o main $(MAIN_OBJ)

recuperar.o:     headers/recuperar.h
distribuir.o:    headers/distribuir.h
utils.o:         headers/utils.h

PHONY = all clean

clean:
	rm -rf $(MAIN_OBJ) main a.out