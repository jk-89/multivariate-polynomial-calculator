/** @file
 * Implementacja modułu odpowiedzialnego za działanie funkcji, które udostępnia
 * kalkulator.
 *
 * @author Jan Kwiatkowski
 */

#include "calc_functions.h"
#include "poly.h"
#include "poly_stack.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>

bool Zero(Stack *stack) {
    Poly poly = PolyZero();
    StackPush(stack, &poly);
    return true;
}

bool IsCoeff(Stack *stack) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackTop(stack);
    PrintlnBool(PolyIsCoeff(top));
    return true;
}

bool IsZero(Stack *stack) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackTop(stack);
    PrintlnBool(PolyIsZero(top));
    return true;
}

bool Clone(Stack *stack) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackTop(stack);
    Poly poly = PolyClone(top);
    StackPush(stack, &poly);
    return true;
}

bool Add(Stack *stack) {
    if (StackUnderflow(stack, 2))
        return false;
    Poly *top = StackPop(stack);
    Poly *prev_top = StackPop(stack);
    Poly poly = PolyAdd(top, prev_top);
    PolyDestroy(top);
    PolyDestroy(prev_top);
    StackPush(stack, &poly);
    return true;
}

bool Mul(Stack *stack) {
    if (StackUnderflow(stack, 2))
        return false;
    Poly *top = StackPop(stack);
    Poly *prev_top = StackPop(stack);
    Poly poly = PolyMul(top, prev_top);
    PolyDestroy(top);
    PolyDestroy(prev_top);
    StackPush(stack, &poly);
    return true;
}

bool Neg(Stack *stack) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackPop(stack);
    Poly poly = PolyNeg(top);
    PolyDestroy(top);
    StackPush(stack, &poly);
    return true;
}

bool Sub(Stack *stack) {
    if (StackUnderflow(stack, 2))
        return false;
    Poly *top = StackPop(stack);
    Poly *prev_top = StackPop(stack);
    Poly poly = PolySub(top, prev_top);
    PolyDestroy(top);
    PolyDestroy(prev_top);
    StackPush(stack, &poly);
    return true;
}

bool IsEq(Stack *stack) {
    if (StackUnderflow(stack, 2))
        return false;
    Poly *top = StackTop(stack);
    Poly *prev_top = StackPrevTop(stack);
    PrintlnBool(PolyIsEq(top, prev_top));
    return true;
}

bool Deg(Stack *stack) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackTop(stack);
    printf("%d\n", PolyDeg(top));
    return true;
}

bool DegBy(Stack *stack, size_t idx) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackTop(stack);
    printf("%d\n", PolyDegBy(top, idx));
    return true;
}

bool At(Stack *stack, poly_coeff_t x) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackPop(stack);
    Poly poly = PolyAt(top, x);
    PolyDestroy(top);
    StackPush(stack, &poly);
    return true;
}

bool Print(Stack *stack) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *top = StackTop(stack);
    PolyPrint(top);
    printf("\n");
    return true;
}

bool Pop(Stack *stack) {
    if (StackUnderflow(stack, 1))
        return false;
    Poly *poly = StackPop(stack);
    PolyDestroy(poly);
    return true;
}

bool Compose(Stack *stack, size_t k) {
    if (stack->size <= k)
        return false;

    Poly *variables = SafeMalloc((k + 1) * sizeof(Poly));
    variables[k] = *StackPop(stack);
    for (size_t i = 0; i < k; i++)
        variables[k - i - 1] = *StackPop(stack);
    Poly composed = PolyCompose(&variables[k], k, variables);
    StackPush(stack, &composed);

    for (size_t i = 0; i <= k; i++)
        PolyDestroy(&variables[i]);
    free(variables);
    return true;
}
