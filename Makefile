CC=gcc
CFLAGS=-I./src/
TARGETS=datamap
DEPS =./lib/cJSON.h
OBJ=./src/datamap.o ./lib/cJSON.o ./src/DBfunctions.c ./src/connection.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

datamap: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lm -lssl


clean:
	rm -f $(TARGETS) lib/*.o

