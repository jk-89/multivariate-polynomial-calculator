/** @file
 * Interfejs modułu odpowiedzialnego za przetworzenie wczytanej linii.
 *
 * @author Jan Kwiatkowski
 */

#ifndef POLYNOMIALS_PROCESS_LINE_H
#define POLYNOMIALS_PROCESS_LINE_H

#include "poly_stack.h"

/**
 * Sprawdza czy stan errno jest równy 0, a następnie ustawia jego stan na 0.
 * Informuje o błędach związanych z np. wywołaniem funkcji getline czy też
 * niepoprawnych argumentach w funkcjach, które oczekują liczby na wejściu.
 * @return stan errno
 */
bool CheckErrno();

/**
 * Przetwarza jeden wiersz wczytany na wejściu.
 * Sprawdza czy na wejściu podano komendę lub wielomian i wywołuje odpowiednią
 * funkcję na stosie @p stack.
 * @param[in] index : numer przetwarzanego wiersza
 * @param[in] read_characters : liczba wczytanych znaków
 * @param[in] input : początek ciągu wczytanych znaków
 * @param[in,out] stack : stos
 */
void ProcessInput(const size_t *index, size_t *read_characters,
                  char *input, Stack *stack);

#endif //POLYNOMIALS_PROCESS_LINE_H
