#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codepages.h"

FILE* inputFile;
FILE* outputFile;
char *codepage;
int finish(char *msg, int status);

int main(int argc, char** argv) {
    if(argc != 4) {
        return finish("USAGE: <input file> <CODEPAGE: cp1251/koi8/iso> <output file>", EXIT_FAILURE);
    }

    inputFile = fopen(argv[1], "r");
    codepage = argv[2];
    outputFile = fopen(argv[3], "w+");
    if(!outputFile) {
        return finish("Failed to create outputFile!", EXIT_FAILURE);
    }
    if(!inputFile) {
        return finish("Failed to open inputFile!", EXIT_FAILURE);
    }

    const uint32_t *inputCodepagePtr;
    if(strcmp(codepage, "cp1251") == 0) {
        inputCodepagePtr = codepage_cp1251;
    } else if(strcmp(codepage, "koi8") == 0) {
        inputCodepagePtr = codepage_koi8;
    } else if(strcmp(codepage, "iso") == 0) {
        inputCodepagePtr = codepage_iso;
    } else {
        return finish("Incorrect codepage!", EXIT_FAILURE);

    }



    int s, i=0;
    while ((s = fgetc(inputFile)) != EOF) {
        uint32_t cp = inputCodepagePtr[s];
        if (cp < 0x80)
        {
            fputc(cp, outputFile);
        } else if (cp < 0x800)
        {
            fputc(0xC0 | (cp >> 6),   outputFile);  /* байт 1: 110xxxxx */
            fputc(0x80 | (cp & 0x3F), outputFile);  /* байт 2: 10xxxxxx */
            //fputc(cp >> 6, outputFile);

        } else if (cp >= 0x800)
        {
            fputc(0x80 | (cp >> 4), outputFile);
        }

        printf("s=%d s=%02X  cp=%04X\n", s, s, cp);

        //fputc((int)inputCodepagePtr[s], outputFile);

        //i++;if (i>10) break;
    }


    return finish("", EXIT_SUCCESS);
}

int finish(char *msg, int status) {
    printf("%s\n", msg);
    if (inputFile) fclose(inputFile);
    if (outputFile) fclose(outputFile);

    return status;
}
