OUTFILE=main
all:
	gcc -Wall -Wextra -Wpedantic -std=c11 main.c -o $(OUTFILE)

test:
	valgrind ./$(OUTFILE)

preprocess:
	gcc -E -c main.c -o main.i

clean:
	rm -rf ./$(OUTFILE) ./$(OUTFILE).i ./$(OUTFILE).s
