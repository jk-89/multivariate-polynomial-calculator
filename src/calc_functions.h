/** @file
 * Interfejs modułu odpowiedzialnego za działanie funkcji, które udostępnia
 * kalkulator.
 * Każda z udostępnianych przez ten moduł funkcji zwraca wartość logiczną
 * informującą, czy udało się wykonać daną funkcję czy też wystąpił błąd
 * podczas próby jej wykonania ze względu na zbyt małą liczbę wielomianów
 * znajdujących się na stosie.
 *
 * @author Jan Kwiatkowski
 */

#ifndef POLYNOMIALS_CALC_FUNCTIONS_H
#define POLYNOMIALS_CALC_FUNCTIONS_H

#include "poly_stack.h"

/**
 * Wstawia na wierzchołek stosu wielomian zerowy.
 * @param[in,out] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Zero(Stack *stack);

/**
 * Sprawdza czy wielomian z wierzchołka stosu jest współczynnikiem.
 * Wypisuje stosowny komunikat na standardowe wyjście.
 * @param[in] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool IsCoeff(Stack *stack);

/**
 * Sprawdza czy wielomian z wierzchołka stosu jest wielomianem zerowym.
 * Wypisuje stosowny komunikat na standardowe wyjście.
 * @param[in] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool IsZero(Stack *stack);

/**
 * Kopiuje wielomian z wierzchołka stosu i wstawia kopię na stos.
 * @param[in,out] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Clone(Stack *stack);

/**
 * Dodaje do siebie dwa wielomiany z góry stosu, wstawia ich sumę na stos.
 * @param[in,out] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Add(Stack *stack);

/**
 * Mnoży przez siebie dwa wielomiany z góry stosu, wstawia ich iloczyn na stos.
 * @param[in,out] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Mul(Stack *stack);

/**
 * Neguje wielomian z wierzchołka stosu.
 * @param[in,out] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Neg(Stack *stack);

/**
 * Odejmuje od wielomianu z wierzchołka stosu wielomian znajdujący się
 * bezpośrednio pod nim, wstawia różnicę na stos.
 * @param[in,out] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Sub(Stack *stack);

/**
 * Sprawdza czy dwa wielomany z wierzchu stosu są sobie równe.
 * Wypisuje stosowny komunikat na standardowe wyjście.
 * @param[in] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool IsEq(Stack *stack);

/**
 * Wyznacza stopień wielomianu z wierzchołka stosu.
 * Wypisuje ten stopień na standardowe wyjście.
 * @param[in] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Deg(Stack *stack);

/**
 * Wyznacza stopień wielomianu z wierzchołka stosu ze względu na zmienną
 * o numerze @p idx.
 * Wypisuje ten stopień na standardowe wyjście.
 * @param[in] stack : stos
 * @param[in] idx : numer zmiennej
 * @return czy udało się poprawnie wykonać funkcję
 */
bool DegBy(Stack *stack, size_t idx);

/**
 * Wyznacza wartość wielomianu z wierzchołka stosu w punkcie @p x, usuwa
 * wielomian z wierzchołka stosu i wstawia na stos wielomian wyznaczony
 * na początku tej funkcji.
 * @param[in] stack : stos
 * @param[in] x : punkt, w którym liczona jest wartość
 * @return czy udało się poprawnie wykonać funkcję
 */
bool At(Stack *stack, poly_coeff_t x);

/**
 * Wypisuje wielomian z wierzchołka stosu.
 * @param[in] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Print(Stack *stack);

/**
 * Usuwa wielomian z wierzchołka stosu.
 * @param[in,out] stack : stos
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Pop(Stack *stack);

/**
 * Pod pierwsze @p k zmiennych wielomianu z wierzchołka stosu wstawia @p k
 * wielomianów znajdujących się pod nim na stosie.
 * @param[in,out] stack : stos
 * @param[in] k : liczba zmiennych
 * @return czy udało się poprawnie wykonać funkcję
 */
bool Compose(Stack *stack, size_t k);

#endif //POLYNOMIALS_CALC_FUNCTIONS_H
