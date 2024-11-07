/** @file
 * Implementacja modułu odpowiedzialnego za działania na wielomianach rzadkich
 * wielu zmiennych.
 * W pliku zachowane są następujące niezmienniki:
 * - tablica jednomianów każdego z wielomianów posortowana jest rosnąco
 * względem ich wykładników,
 * - w jednej tablicy jednomianów znajdują się jednomiany o parami różnych
 * wykładnikach,
 * - jeśli wielomian może być uproszczony do wielomianu stałego, to jest
 * upraszczany.
 * @author Jan Kwiatkowski
 */

#include "poly.h"
#include "utilities.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

/**
 * Funkcja dodająca lub mnożąca wielomian stały przez wielomian niestały.
 * @param[in] p : wielomian stały
 * @param[in] q : wielomian, który nie jest stały
 * @param[in] add : czy należy dodać czy też pomnożyć wielomiany
 * @return jeśli @p add = true zwraca @f$p + q@f$, w przeciwnym przypadku
 * zwraca @f$p * q@f$
 */
static Poly HandleOneCoeffAddOrMul(const Poly *p, const Poly *q, bool add) {
    assert(PolyIsCoeff(p) && !PolyIsCoeff(q));

    Mono single[1];
    single[0] = MonoFromPoly(p, 0);
    Poly new_p = {.size = 1, .arr = single};

    return add ? PolyAdd(&new_p, q) : PolyMul(&new_p, q);
}

/**
 * Zmienia długość tablicy jednomianów w @p p na @p arr_size lub, jeśli @p p
 * jest tożsamościowo równy wielomianowi stałemu, zmienia @p p w odpowiedni
 * wielomian stały.
 * Jest to funkcja pomocnicza do @ref PolyAdd(const Poly *p, const Poly *q)
 * oraz do @ref PolyMul(const Poly *p, const Poly *q), która odpowiednio
 * modyfikuje wielomian powstały w wyniku wywołania tych funkcji.
 * @param[in,out] p : wielomian, który nie jest stały
 * @param[in] arr_size : nowa długość tablicy jednomianów w @p p
 */
static void PolyChangeIfCoeff(Poly *p, const size_t *arr_size) {
    assert(!PolyIsCoeff(p));

    if (*arr_size == 0) {
        PolyDestroy(p);
        *p = PolyZero();
    } else if (*arr_size == 1 && p->arr[0].exp == 0 &&
               PolyIsCoeff(&p->arr[0].p)) {
        Poly coeff_ret = PolyClone(&p->arr[0].p);
        PolyDestroy(p);
        *p = coeff_ret;
    } else {
        p->size = *arr_size;
    }
}

/**
 * Klonuje jednomian z @p p do @p poly_ret o ile wielomian, który zawiera ten
 * jednomian nie jest tożsamościowo równy 0.
 * Klonuje jednomian z @p p znajdujący się w tablicy jednomianów na indeksie
 * @p p_ptr i zapisuje go w tablicy jednomianów @p poly_ret na indeksie @p ptr.
 * Następnie zwiększa oba indeksy o 1.
 * @param[in,out] poly_ret : modyfikowany wielomian
 * @param[in,out] ptr : indeks w tablicy jednomianów @p poly_ret, w którym
 * zapisujemy skopiowany jednomian
 * @param[in] p : wielomian, z którego kopiujemy jednomian
 * @param[in,out] p_ptr : indeks w tablicy jednomianów @p p, z którego
 * kopiujemy jednomian
 */
static void MonoCloneIfNotZero(Poly *poly_ret, size_t *ptr, const Poly *p,
                               size_t *p_ptr) {
    assert(!PolyIsCoeff(p) && p->size > *p_ptr);
    assert(!PolyIsCoeff(poly_ret) && poly_ret->size > *ptr);

    if (PolyIsZero(&p->arr[*p_ptr].p)) {
        *p_ptr = *p_ptr + 1;
        return;
    }

    poly_ret->arr[*ptr] = MonoClone(&p->arr[*p_ptr]);
    *ptr = *ptr + 1;
    *p_ptr = *p_ptr + 1;
}

void PolyDestroy(Poly *p) {
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < p->size; i++)
            MonoDestroy(&p->arr[i]);
        free(p->arr);
    }
}

Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p))
        return PolyFromCoeff(p->coeff);

    Poly ret_poly = CreateNotCoeffPoly(p->size);
    for (size_t i = 0; i < p->size; i++)
        ret_poly.arr[i] = MonoClone(&p->arr[i]);

    return ret_poly;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyFromCoeff(p->coeff + q->coeff);
    if (PolyIsCoeff(p))
        return HandleOneCoeffAddOrMul(p, q, true);
    if (PolyIsCoeff(q))
        return HandleOneCoeffAddOrMul(q, p, true);

    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));
    assert(PolyIsSorted(p) && PolyIsSorted(q));

    Poly poly_ret = CreateNotCoeffPoly(p->size + q->size);
    size_t ret_arr_size = 0;

    // przechodzimy naraz po jednomianach z p i q za każdym razem dodając
    // do poly_ret.arr ten o mniejszym wykładniku (chyba że wykładniki są
    // równe, wtedy sumujemy wielomiany z tych jednomianów)
    size_t p_ptr = 0, q_ptr = 0;
    while (p_ptr < p->size || q_ptr < q->size) {
        if (p_ptr == p->size) {
            MonoCloneIfNotZero(&poly_ret, &ret_arr_size, q, &q_ptr);
        } else if (q_ptr == q->size) {
            MonoCloneIfNotZero(&poly_ret, &ret_arr_size, p, &p_ptr);
        } else {
            if (p->arr[p_ptr].exp < q->arr[q_ptr].exp) {
                MonoCloneIfNotZero(&poly_ret, &ret_arr_size, p, &p_ptr);
            } else if (p->arr[p_ptr].exp > q->arr[q_ptr].exp) {
                MonoCloneIfNotZero(&poly_ret, &ret_arr_size, q, &q_ptr);
            } else {
                poly_ret.arr[ret_arr_size].p = PolyAdd(&p->arr[p_ptr].p,
                                                       &q->arr[q_ptr].p);
                poly_ret.arr[ret_arr_size].exp = p->arr[p_ptr].exp;

                // zwiększamy rozmiar tablicy tylko wtedy, gdy uzyskaliśmy
                // niezerowy wielomian (zerowy wielomian może być później
                // nadpisywany)
                if (!PolyIsZero(&poly_ret.arr[ret_arr_size].p))
                    ret_arr_size++;

                p_ptr++;
                q_ptr++;
            }
        }
    }

    PolyChangeIfCoeff(&poly_ret, &ret_arr_size);

    PolySort(&poly_ret);
    return poly_ret;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0)
        return PolyZero();

    Mono copy[count];
    Poly from = {.size = count, .arr = copy};
    for (size_t i = 0; i < count; i++)
        from.arr[i] = monos[i];

    PolySort(&from);

    Poly poly_ret = CreateNotCoeffPoly(count);
    size_t size = 0;
    poly_ret.arr[0] = from.arr[0];

    // przechodzimy po jednomianach posortowanych po wykładnikach, sumujemy
    // współczynniki jednomianów o tych samych wykładnikach
    for (size_t i = 1; i < count; i++) {
        if (from.arr[i].exp == from.arr[i - 1].exp) {
            Poly prev = poly_ret.arr[size].p;
            poly_ret.arr[size].p = PolyAdd(&prev, &from.arr[i].p);
            MonoDestroy(&from.arr[i]);
            PolyDestroy(&prev);
        }
        else {
            if (!PolyIsZero(&poly_ret.arr[size].p)) {
                poly_ret.arr[size].exp = from.arr[i - 1].exp;
                size++;
            }
            poly_ret.arr[size] = from.arr[i];
        }
    }

    // sprawdzamy czy nie trzeba dodać ostatniego jednomianu z tablicy
    if (!PolyIsZero(&poly_ret.arr[size].p))
        size++;

    PolyChangeIfCoeff(&poly_ret, &size);
    assert(PolyIsSorted(&poly_ret));

    return poly_ret;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyFromCoeff(p->coeff * q->coeff);
    if (PolyIsCoeff(p))
        return HandleOneCoeffAddOrMul(p, q, false);
    if (PolyIsCoeff(q))
        return HandleOneCoeffAddOrMul(q, p, false);

    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));
    assert(PolyIsSorted(p) && PolyIsSorted(q));

    if (p->size > q->size) {
        return PolyMul(q, p);
    }

    Poly poly_ret = PolyZero();

    for (size_t i = 0; i < p->size; i++) {
        Poly processed = CreateNotCoeffPoly(q->size);
        for (size_t j = 0; j < q->size; j++) {
            processed.arr[j].exp = p->arr[i].exp + q->arr[j].exp;
            processed.arr[j].p = PolyMul(&p->arr[i].p, &q->arr[j].p);
        }

        Poly poly_new = PolyAdd(&poly_ret, &processed);
        PolyDestroy(&poly_ret);
        PolyDestroy(&processed);
        poly_ret = poly_new;
    }

    return poly_ret;
}

Poly PolyNeg(const Poly *p) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(-1 * p->coeff);
    }

    Poly poly_ret = CreateNotCoeffPoly(p->size);
    for (size_t i = 0; i < poly_ret.size; i++) {
        poly_ret.arr[i].p = PolyNeg(&p->arr[i].p);
        poly_ret.arr[i].exp = p->arr[i].exp;
    }

    return poly_ret;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly neg_q = PolyNeg(q);
    Poly poly_ret = PolyAdd(p, &neg_q);
    PolyDestroy(&neg_q);

    return poly_ret;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsZero(p))
        return -1;

    if (PolyIsCoeff(p))
        return 0;

    poly_exp_t ret = 0;
    for (size_t i = 0; i < p->size; i++) {
        if (var_idx == 0) {
            ret = PolyExpMax(ret, p->arr[i].exp);
        } else {
            ret = PolyExpMax(ret, PolyDegBy(&p->arr[i].p, var_idx - 1));
        }
    }

    return ret;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p))
        return -1;

    if (PolyIsCoeff(p))
        return 0;

    poly_exp_t ret = 0;
    for (size_t i = 0; i < p->size; i++)
        ret = PolyExpMax(ret, p->arr[i].exp + PolyDeg(&p->arr[i].p));

    return ret;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) != PolyIsCoeff(q))
        return false;

    if (PolyIsCoeff(p))
        return p->coeff == q->coeff;

    if (p->size != q->size)
        return false;

    for (size_t i = 0; i < p->size; i++) {
        if (p->arr[i].exp != q->arr[i].exp ||
            !PolyIsEq(&p->arr[i].p, &q->arr[i].p)) {
            return false;
        }
    }

    return true;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p))
        return PolyClone(p);

    Poly poly_ret = PolyZero();
    for (size_t i = 0; i < p->size; i++) {
        Poly coeff = PolyFromCoeff(FastPow(x, p->arr[i].exp));
        Poly mul_poly = PolyMul(&p->arr[i].p, &coeff);
        Poly poly_new = PolyAdd(&poly_ret, &mul_poly);

        PolyDestroy(&mul_poly);
        PolyDestroy(&poly_ret);

        poly_ret = poly_new;
    }

    return poly_ret;
}

void PolyPrint(const Poly *p) {
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
        return;
    }
    for (size_t i = 0; i < p->size; i++) {
        if (i != 0)
            printf("+");
        printf("(");
        PolyPrint(&p->arr[i].p);
        printf(",%d)", p->arr[i].exp);
    }
}

Poly PolyOwnMonos(size_t count, Mono *monos) {
    if (count == 0 || monos == NULL) {
        free(monos);
        return PolyZero();
    }

    Poly poly_ret = {.size = count, .arr = monos};

    PolySort(&poly_ret);

    size_t size = 0;

    // przechodzimy po jednomianach posortowanych po wykładnikach, sumujemy
    // współczynniki jednomianów o tych samych wykładnikach
    for (size_t i = 1; i < count; i++) {
        if (poly_ret.arr[i].exp == poly_ret.arr[i - 1].exp) {
            Poly prev = poly_ret.arr[size].p;
            poly_ret.arr[size].p = PolyAdd(&prev, &poly_ret.arr[i].p);
            PolyDestroy(&poly_ret.arr[i].p);
            PolyDestroy(&prev);
        }
        else {
            if (!PolyIsZero(&poly_ret.arr[size].p)) {
                poly_ret.arr[size].exp = poly_ret.arr[i - 1].exp;
                size++;
            }
            poly_ret.arr[size] = poly_ret.arr[i];
        }
    }
    // sprawdzamy czy nie trzeba dodać ostatniego jednomianu z tablicy
    if (!PolyIsZero(&poly_ret.arr[size].p))
        size++;

    poly_ret.size = size;

    PolyChangeIfCoeff(&poly_ret, &size);
    assert(PolyIsSorted(&poly_ret));
    return poly_ret;
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL)
        return PolyZero();

    // skopiowanie zawartości tablicy, a następnie wywołanie PolyOwnMonos()
    Mono *monos_copy = SafeMalloc(count * sizeof(Mono));
    for (size_t i = 0; i < count; i++)
        monos_copy[i] = MonoClone(&monos[i]);

    return PolyOwnMonos(count, monos_copy);
}

/**
 * Dla danej zmiennej w wielomianie (o indeksie mniejszym niż @p k) znajduje
 * najwyższą potęgę, w której ona występuje.
 * @param[in] p : wielomian
 * @param[in,out] max_exp : tablica najwyższych potęg
 * @param[in] k : rozmiar tablicy
 * @param[in] depth : indeks zmiennej, która jest aktualnie rozważana
 */
static void MaxExpFill(const Poly *p, poly_exp_t *max_exp, size_t k,
                       size_t depth) {
    if (PolyIsCoeff(p) || depth >= k)
        return;

    for (size_t i = 0; i < p->size; i++) {
        MaxExpFill(&p->arr[i].p, max_exp, k, depth + 1);
        max_exp[depth] = PolyExpMax(p->arr[i].exp, max_exp[depth]);
    }
}

/**
 * Dla danego wykładnika i tablicy potęg danego wielomianu odpowiednio
 * przemnaża kolejne potęgi aby uzyskać potęgę równą wykładnikowi.
 * @param powers : potęgi wielomianu
 * @param exp : docelowa potęga
 * @return wielomian podniesiony do potęgi @p exp
 */
static Poly PolyFastPow(const Poly *powers, poly_exp_t exp) {
    assert(exp >= 0);
    Poly poly_ret = PolyFromCoeff(1);
    if (exp == 0)
        return poly_ret;

    size_t index = 0;
    poly_exp_t power = 1;
    while (power <= exp / 2 && power * 2 <= exp) {
        power *= 2;
        index++;
    }

    for (size_t i = index; ; i--) {
        if (power <= exp) {
            Poly prev = poly_ret;
            poly_ret = PolyMul(&poly_ret, &powers[i]);
            PolyDestroy(&prev);
            exp -= power;
        }
        power /= 2;
        if (i == 0)
            break;
    }

    return poly_ret;
}

/**
 * Funkcja pomocniczna do @ref PolyCompose(const Poly *p, size_t k,
 * const Poly q[]).
 * Przyjmuje dodatkowy argument informujący, która w kolejności zmienna jest
 * aktualnie rozważana w wielomianie @p p.
 * @param[in] p : wielomian
 * @param[in] k : ilość podstawianych wielomianów
 * @param[in] powers : tablica potęg wielomianów, które należy podstawić
 * @param[in] depth : indeks rozważanej zmiennej w wielomianie @p p
 * @return wielomian powstały w wyniku złożenia
 */
static Poly PolyComposeHelper(const Poly *p, size_t k, Poly **powers,
                              size_t depth) {
    if (PolyIsCoeff(p))
        return PolyClone(p);

    if (depth >= k) {
        if (p->arr[0].exp != 0)
            return PolyZero();
        else
            return PolyComposeHelper(&p->arr[0].p, k, powers, depth + 1);
    }

    Poly poly_ret = PolyZero();
    for (size_t i = 0; i < p->size; i++) {
        Poly poly = PolyComposeHelper(&p->arr[i].p, k, powers, depth + 1);
        Poly power_poly = PolyFastPow(powers[depth], p->arr[i].exp);
        Poly mul_poly = PolyMul(&power_poly, &poly);
        Poly prev = poly_ret;
        poly_ret = PolyAdd(&poly_ret, &mul_poly);
        PolyDestroy(&prev);
        PolyDestroy(&power_poly);
        PolyDestroy(&mul_poly);
        PolyDestroy(&poly);
    }

    return poly_ret;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
    // znalezienie maksymalnych wykładników dla danej zmiennej
    poly_exp_t *max_exp = SafeMalloc(k * sizeof(poly_exp_t));
    for (size_t i = 0; i < k; i++)
        max_exp[i] = 0;
    MaxExpFill(p, max_exp, k, 0);

    // obliczenie wykorzystywanych później potęg
    Poly **powers = SafeMalloc(k * sizeof(Poly*));
    for (size_t i = 0; i < k; i++) {
        size_t size = PowersOfTwo(max_exp[i]);
        powers[i] = SafeMalloc(size * sizeof(Poly));
        powers[i][0] = PolyClone(&q[i]);
        for (size_t j = 1; j < size; j++)
            powers[i][j] = PolyMul(&powers[i][j - 1], &powers[i][j - 1]);
    }

    // właściwe składanie
    Poly poly_ret = PolyComposeHelper(p, k, powers, 0);
    for (size_t i = 0; i < k; i++) {
        for (size_t j = 0; j < PowersOfTwo(max_exp[i]); j++)
            PolyDestroy(&powers[i][j]);
        free(powers[i]);
    }
    free(powers);
    free(max_exp);
    return poly_ret;
}
