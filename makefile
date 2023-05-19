all: feab

feab: *.c *.h
	gcc -o feab *.c -ansi -Wall -Wpedantic -lSDL2

run: all
	./feab

clean:
	rm feab
	rm *.rom
