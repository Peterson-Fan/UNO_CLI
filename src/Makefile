CC=gcc
CFLAGS=-O2 -Werror -Wall -Wextra -Wconversion -Wvla -Wpedantic -std=c11 -lm
SOURCE=main.c data_structure.c game.c interaction.c

.PHONY: clean all

all: onecard

check-asan-lsan-ubsan: $(SOURCE)
	clang -DDEBUG -g -fsanitize=address,leak,undefined,integer,bounds,float-divide-by-zero,float-cast-overflow -fno-omit-frame-pointer -fno-sanitize-recover=all -o check-asan-lsan-ubsan $(SOURCE)

check-msan: $(SOURCE)
	clang -DDEBUG -g -fsanitize=memory -fno-omit-frame-pointer -fsanitize-memory-track-origins -fPIE -pie -o check-msan $(SOURCE)

onecard: $(SOURCE)
	$(CC) -o onecard $(SOURCE) $(CFLAGS)

clean:
	$(RM) *.o *~ onecard
