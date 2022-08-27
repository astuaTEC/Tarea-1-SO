#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// https://www.programiz.com/c-programming/examples/vowel-consonant-frequency-string
int consonantCounter(char *file)
{

    int consonant;
    FILE *fp;
    char c;

    // initialize all variables to 0
    consonant = 0;

    fp = fopen(file, "r"); // 'r' opens the file in read mode

    if (fp == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }

    printf("READING THE CONTENTS OF THE FILE [ %s ]\n", file);

    while ((c = fgetc(fp)) != EOF)
    {
        // if it is not a vowel and if it is an alphabet, it is a consonant
        if ((c >= 'a' && c <= 'z'))
        {
            ++consonant;
        }
    }

    printf("Consonants: %d\n", consonant);

    fclose(fp);

    return consonant;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "Filename not provided. Program terminated\n");
        exit(1);
    }

    char *filename;
    filename = argv[1];
    consonantCounter(filename);
}