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

    
    codepage = argv[2];

    const uint32_t *inputCodepagePtr;
    if(strcmp(codepage, "cp1251") == 0) {
        inputCodepagePtr = codepage_cp1251;
    } else if(strcmp(codepage, "koi8") == 0) {
        inputCodepagePtr = codepage_koi8;
    } else if(strcmp(codepage, "iso") == 0) {
        inputCodepagePtr = codepage_iso;
    } else {
        return finish("Incorrect codepage! Needed: cp1251/koi8/iso", EXIT_FAILURE);

    }

    outputFile = fopen(argv[3], "w+");
    if(!outputFile) {
        return finish("Failed to create outputFile!", EXIT_FAILURE);
    }
    inputFile = fopen(argv[1], "r");
    if(!inputFile) {
        return finish("Failed to open inputFile!", EXIT_FAILURE);
    }



    int s;//, i=0;
    while ((s = fgetc(inputFile)) != EOF) {
        uint32_t cp = inputCodepagePtr[s];
        if (cp < 0x80)
        {
            fputc(cp, outputFile);
        } else if (cp < 0x800)
        {
            fputc(0xC0 | (cp >> 6),   outputFile);  /* байт 1: 110xxxxx */
            fputc(0x80 | (cp & 0x3F), outputFile);  /* байт 2: 10xxxxxx */

        } else if (cp >= 0x800)
        {
            fputc(0xE0 | (cp >> 12),   outputFile);
            fputc(0x80 | ((cp >> 6) & 0x3F),   outputFile);
            fputc(0x80 | (cp & 0x3F), outputFile);
        }

        //printf("s=%d s=%02X  cp=%04X\n", s, s, cp);

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
