/** @file
 * Implementacja modułu udostępniającego ogólne funkcje, przydające się
 * w obrębie całego projektu.
 *
 * @author Jan Kwiatkowski
 */

#include "utilities.h"
#include "poly.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/**
 * Komparator porównujący dwie struktury Mono, za większą uznaje tę o większym
 * wykładniku.
 * Zwracana wartość odpowiada konwencji przyjętej w komparatorach języka C.
 * Jeśli pierwszy jednomian ma mniejszy wykładnik zwraca -1, jeśli pierwszy
 * jednomian ma większy wykładnik zwraca 1, a jeśli wykładniki są równe
 * zwraca 0.
 * @param a : pierwszy jednomian
 * @param b : drugi jednomian
 * @return informacja, który z jednomianów ma większy wykładnik
 */
static int MonoCompByExp(const void *a, const void *b) {
    Mono *mono_a = (Mono*)a;
    Mono *mono_b = (Mono*)b;

    if (mono_a->exp < mono_b->exp) {
        return -1;
    } else if (mono_a->exp > mono_b->exp) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Wywołuje funkcję calloc() dla typu Mono i sprawdza, czy alokacja
 * przebiegła poprawnie.
 * @param[in] n : ilość alokowanych elementów
 * @return wskaźnik na zaalokowany blok w pamięci
 */
static Mono* SecureCallocMono(size_t n) {
    Mono *ret = calloc(n, sizeof(Mono));
    if (!ret)
        exit(1);
    return ret;
}

void* SafeMalloc(size_t n) {
    void *ptr = malloc(n);
    if (!ptr)
        exit(1);
    return ptr;
}

void* SafeRealloc(void *ptr, size_t n) {
    ptr = realloc(ptr, n);
    if (!ptr)
        exit(1);
    return ptr;
}

poly_exp_t PolyExpMax(poly_exp_t a, poly_exp_t b) {
    return a > b ? a : b;
}

poly_coeff_t FastPow(poly_coeff_t coeff, poly_exp_t exp) {
    assert(exp >= 0);
    poly_coeff_t ret = 1;

    while (exp > 0) {
        if (exp % 2 == 1)
            ret *= coeff;
        coeff *= coeff;
        exp /= 2;
    }

    return ret;
}

Poly CreateNotCoeffPoly(size_t n) {
    assert(n != 0);
    Poly ret_poly = {.size = n, .arr = SecureCallocMono(n)};
    return ret_poly;
}

void PolySort(Poly *p) {
    if (PolyIsCoeff(p))
        return;

    qsort(p->arr, p->size, sizeof(Mono), MonoCompByExp);
}

bool PolyIsSorted(const Poly *p) {
    if (PolyIsCoeff(p))
        return true;

    for (size_t i = 1; i < p->size; i++) {
        if (p->arr[i].exp <= p->arr[i - 1].exp)
            return false;
    }

    return true;
}

void PrintlnBool(bool b) {
    if (b)
        printf("1\n");
    else
        printf("0\n");
}

size_t PowersOfTwo(size_t k) {
    size_t ret = 0;
    size_t power = 1;
    while (power <= k) {
        power *= 2;
        ret++;
    }
    if (ret == 0)
        ret = 1;
    return ret;
}
