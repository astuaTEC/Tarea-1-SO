#include <stdio.h>
#include <ctype.h>
#include "consonantCounter.h"

// https://www.programiz.com/c-programming/examples/vowel-consonant-frequency-string
int consonantCounter(char *string)
{

    int consonant;

    // initialize all variables to 0
    consonant = 0;

    // loop through each character of the string
    for (int i = 0; string[i] != '\0'; ++i)
    {

        // convert character to lowercase
        string[i] = tolower(string[i]);

        // if it is not a vowel and if it is an alphabet, it is a consonant
        if ((string[i] >= 'a' && string[i] <= 'z'))
        {
            ++consonant;
        }
    }

    return consonant;
}