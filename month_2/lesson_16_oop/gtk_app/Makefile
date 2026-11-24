CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 `pkg-config --cflags gtk4`
LIBS    = `pkg-config --libs gtk4`

all: gtk_app

files.o: modules/files.c modules/files.h
	gcc $(CFLAGS) -c modules/files.c -o files.o

main.o: main.c modules/files.h
	gcc $(CFLAGS) -c main.c -o main.o

gtk_app: main.o files.o
	gcc main.o files.o $(LIBS) -o gtk_app
