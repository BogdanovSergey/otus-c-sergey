OUTFILE=main
all:
	gcc -Wall -Wextra -Wpedantic -std=c11 llist.c -o $(OUTFILE)

test:
	valgrind ./$(OUTFILE)

preprocess:
	gcc -E -c main.c -o main.i

clean:Ж
	rm -rf ./$(OUTFILE) ./$(OUTFILE).i ./$(OUTFILE).s
