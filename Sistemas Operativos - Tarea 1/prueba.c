#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(){

    char resp;

    printf("Digite una letra: ");
    scanf("%c", &resp);

    printf("Char: %c \n", resp);

    printf("Resultado comparar: %d", strcmp(&resp, "y"));
}