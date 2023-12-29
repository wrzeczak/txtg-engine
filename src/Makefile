build:
	gcc -o app main.c -Wall -pedantic -ggdb -lncurses -fsanitize-recover=address,undefined

run: build
	./app
