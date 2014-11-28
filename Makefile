all: sha3

sha3: keccak.o utils.o main.o
	gcc keccak.o utils.o main.o -o sha3.out -g

keccak.o: keccak.c
	gcc -c keccak.c -std=c99 -Wall -g

utils.o: utils.c
	gcc -c utils.c -std=c99 -Wall -g
	
main.o: main.c
	gcc -c main.c -std=c99 -Wall -g

clean:
	rm -rf *o sha3