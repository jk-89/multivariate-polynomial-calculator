/** @file
 * Implementacja modułu odpowiedzialnego za działanie kalkulatora.
 *
 * @author Jan Kwiatkowski
 */

#define _GNU_SOURCE

#include "poly.h"
#include "poly_stack.h"
#include "process_line.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/**
 * Wczytuje jedną linię z wejścia.
 * @param[in,out] input : wskaźnik na miejsce, w którym zapisywany jest
 * wczytany ciąg znaków
 * @param[out] read_characters : liczba wczytanych przez getline() znaków
 * @param[in,out] getline_size : długość bufora w getline()
 */
static void Read(char **input, ssize_t *read_characters, size_t *getline_size) {
    *read_characters = getline(input, getline_size, stdin);

    // jeśli wczytywanie nie powiodło się ze względu na brak pamięci
    if (!CheckErrno()) {
        exit(1);
    }
}

/**
 * Wczytuje dane i wywołuje odpowiednie funkcje w celu ich przetworzenia.
 * @return kod wyjściowy programu
 */
int main() {
    Stack *stack = StackCreate();
    char *input = NULL;
    size_t getline_size = 0;
    errno = 0;
    size_t lines_counter = 0;

    while (true) {
        ssize_t read_characters;

        Read(&input, &read_characters, &getline_size);

        // nie ma już nic do wczytania
        if (read_characters == -1) {
            break;
        }

        size_t read_chars = (size_t) (read_characters);

        lines_counter++;
        ProcessInput(&lines_counter, &read_chars, input, stack);
    }

    free(input);
    StackClear(stack);

    return 0;
}
