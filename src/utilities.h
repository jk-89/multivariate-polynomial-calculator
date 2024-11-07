/** @file
 * Interfejs modułu udostępniającego ogólne funkcje, przydające się
 * w obrębie całego projektu.
 *
 * @author Jan Kwiatkowski
 */

#ifndef POLYNOMIALS_UTILITIES_H
#define POLYNOMIALS_UTILITIES_H

#include "poly.h"
#include <stddef.h>

/**
 * Wywołuje funkcję malloc() i sprawdza czy alokacja przebiegła poprawnie.
 * @param[in] n : rozmiar alokowanych elementów
 * @return wskaźnik na początek zaalokowanej pamięci
 */
void* SafeMalloc(size_t n);

/**
 * Wywołuje funkcję realloc() i sprawdza czy realokacja przebiegła poprawnie.
 * @param[in,out] ptr : wskaźnik na realokowane miejsce w pamięci
 * @param[in] n : rozmiar alokowanych elementów
 * @return wskaźnik na początek zrealokowanej pamięci
 */
void* SafeRealloc(void *ptr, size_t n);

/**
 * Zwraca większy z dwóch wykładników.
 * @param[in] a : wykładnik
 * @param[in] b : wykładnik
 * @return większy z wykładników @p a @p b
 */
poly_exp_t PolyExpMax(poly_exp_t a, poly_exp_t b);

/**
 * Podnosi całkowity współczynnik @p coeff do nieujemnej, całkowitej
 * potęgi @p exp.
 * Wykorzystuje do tego algorytm znany jako szybkie potęgowanie.
 * @param[in] coeff : podstawa potęgi
 * @param[in] exp : wykładnik potęgi
 * @return : @p coeff^@p exp
 */
poly_coeff_t FastPow(poly_coeff_t coeff, poly_exp_t exp);

/**
 * Tworzy nowy wielomian, który nie jest współczynnikiem.
 * @param[in] n : liczba jednomianów w tworzonym wielomianie
 * @return utworzony wielomian
 */
Poly CreateNotCoeffPoly(size_t n);

/**
 * Sortuje dany wielomian względem wykładników jednomianów, z których składa
 * się ten wielomian.
 * Jeśli wielomian jest wielomianem stałym nie zmienia jego stanu.
 * @param[in,out] p : wielomian, którego jednomiany zostaną posortowane
 */
void PolySort(Poly *p);

/**
 * Sprawdza czy dany wielomian ma posortowane jednomiany w kolejności
 * rosnących wykładników.
 * @param[in] p : wielomian
 * @return czy dany wielomian ma posortowane jednomiany w kolejności
 * rosnących wykładników
 */
bool PolyIsSorted(const Poly *p);

/**
 * Jeśli @p b jest prawdą wypisuje $1, w przeciwnym wypadku wypisuje $0.
 * Po wypisaniu wartości wypisuje znak przejścia do nowej linii.
 * @param[in] b : wartość logiczna
 */
void PrintlnBool(bool b);

/**
 * Liczy liczbę dodatnich potęg liczby 2 mniejszych bądź równych liczbie
 * wejściowej.
 * Dla @p k = 0 zwraca 1.
 * @param[in] k : liczba
 * @return liczba dodatnich potęg liczby 2 mniejszych bądź równych @p k
 */
size_t PowersOfTwo(size_t k);

#endif //POLYNOMIALS_UTILITIES_H