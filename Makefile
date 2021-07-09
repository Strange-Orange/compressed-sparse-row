CC = gcc
CFLAGS = -c -g -Wall
SRCS = $(wildcard ./*.c)
OBJS = $(patsubst ./%.c, ./obj/%.o, $(SRCS))

# Link
bin/compressed-sparse-row.out: $(OBJS)
	@mkdir -p ./bin
	$(CC) $^ -o bin/compressed-sparse-row.out

# Compile
./obj/%.o: ./%.c
	@mkdir -p ./obj
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm ./obj/*.o ./bin/*.out
