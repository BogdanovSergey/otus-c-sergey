OUTFILE=main
all:
	gcc -Wall -Wextra -Wpedantic -std=c11 main.c -o $(OUTFILE) -lcurl

test:
	valgrind ./$(OUTFILE)

clean:
	rm -rf ./$(OUTFILE)
