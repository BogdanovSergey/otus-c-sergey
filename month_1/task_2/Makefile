OUTFILE=main
all:
	gcc -Wall -Wextra -Wpedantic -std=c11 cjson/cJSON.c main.c -o $(OUTFILE) -lcurl -lm

test:
	valgrind ./$(OUTFILE)

preprocess:
	gcc -E -c main.c -lcurl -o main.i

clean:
	rm -rf ./$(OUTFILE) ./$(OUTFILE).i ./$(OUTFILE).s ./result.json
