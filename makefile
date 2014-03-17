CC=gcc
CFLAGS=-I./src/
TARGETS=datamap
DEPS =./lib/cJSON.h
OBJ=./src/datamap.o ./lib/cJSON.o #./src/DBfunctions.o ./src/connection.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

datamap: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lm -lssl

DBfunctions: ./src/DBfunctions.c
	gcc -o DBfunctions ./src/DBfunctions.c -lssl -lm

connection: ./src/connection.c
	gcc -o connection ./src/connection.c -lssl -lm

#datamap: ./src/datamap.c
#	gcc -o datamap ./src/datamap.c -lssl -lm

clean:
	rm -f $(TARGETS) lib/*.o src/*.o


