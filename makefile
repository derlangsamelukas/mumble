
./a.out: src/main.c src/helper.c src/generated.c src/types.c src/eval.c src/std.c src/pacman.c src/memory.c
	gcc $^ -o $@ `pkg-config --libs --cflags sdl2` -lSDL2 -lSDL2_image -lm

run: ./a.out
	./a.out
