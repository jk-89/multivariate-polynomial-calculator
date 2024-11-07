/** @file
 * Interfejs modułu udostępniającego strukturę danych - stos wielomianów.
 *
 * @author Jan Kwiatkowski
 */

#ifndef POLYNOMIALS_POLY_STACK_H
#define POLYNOMIALS_POLY_STACK_H

#include "poly.h"

/** Początkowy rozmiar tablicy utrzymującej stos. */
#define INIT_STACK_ARR_SIZE 16

/**
 * Struktura przechowująca stos wielomianów.
 */
typedef struct Stack {
    /**
     * Tablica wielomianów utrzymująca stos.
     */
    Poly *arr;
    size_t arr_size; ///< rozmiar tablicy @p arr
    size_t size; ///< ilość wielomianów w tablicy @p arr
} Stack;

/**
 * Tworzy nowy stos.
 * @return utworzony stos
 */
Stack* StackCreate();

/**
 * Sprawdza czy stos jest pusty.
 * @param[in] stack : stos
 * @return czy stos jest pusty
 */
bool StackIsEmpty(Stack *stack);

/**
 * Sprawdza czy na stosie jest co najmniej @p size wielomianów.
 * @param[in] stack : stos
 * @param[in] size : ilość wielomianów
 * @return czy na stosie jest co najmniej @p size wielomianów
 */
bool StackUnderflow(Stack *stack, size_t size);

/**
 * Wstawia wielomian na wierzchołek stosu.
 * @param[in,out] stack : stos
 * @param[in] p : wielomian, który będzie wstawiony
 */
void StackPush(Stack *stack, Poly *p);

/**
 * Zwraca wielomian z wierzchołka stosu.
 * @param[in] stack : stos
 * @return wielomian z wierzchołka stosu @p stack
 */
Poly* StackTop(Stack *stack);

/**
 * Zwraca drugi od góry wielomian ze stosu.
 * @param[in] stack : stos
 * @return drugi od góry wielomian ze stosu
 */
Poly* StackPrevTop(Stack *stack);

/**
 * Zwraca wielomian z wierzchołka stosu oraz usuwa go ze stosu.
 * @param[in,out] stack : stos
 * @return wielomian z wierzchołka stosu @p stack
 */
Poly* StackPop(Stack *stack);

/**
 * Usuwa wszystkie wielomiany ze stosu oraz sam stos.
 * @param[in,out] stack : stos
 */
void StackClear(Stack *stack);

#endif //POLYNOMIALS_POLY_STACK_H
