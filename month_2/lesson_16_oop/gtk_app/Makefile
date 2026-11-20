INFILE=main
OUTFILE=a.out
all:
	gcc -Wall -Wextra -Wpedantic -std=c11 -lm $(INFILE).c `pkg-config --cflags --libs gtk4 `  -o $(OUTFILE);

test:
	valgrind ./$(OUTFILE)

preprocess:
	gcc -E -c $(INFILE).c -lcurl -o $(INFILE).i

clean:
	rm -rf ./$(OUTFILE) ./$(OUTFILE).i ./$(OUTFILE).s

.PHONY: test all clean