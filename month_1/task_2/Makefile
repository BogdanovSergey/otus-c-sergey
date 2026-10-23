OUTFILE=main
all:
	gcc -Wall -Wextra -Wpedantic -std=c11 main.c -o $(OUTFILE) -lcurl

test:
	valgrind ./$(OUTFILE)

preprocess:
	gcc -E -c main.c -lcurl -o main.i

clean:
	rm -rf ./$(OUTFILE)
