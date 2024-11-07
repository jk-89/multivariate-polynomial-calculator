/** @file
 * Implementacja modułu udostępniającego stos wielomianów.
 *
 * @author Jan Kwiatkowski
 */

#include "poly_stack.h"
#include "utilities.h"
#include <malloc.h>

/**
 * Zwiększa rozmiar tablicy utrzymującej wielomiany znajdujące się na
 * danym stosie.
 * @param[in,out] stack : stos
 */
static void StackIncreaseSize(Stack *stack) {
    stack->arr_size *= 2;
    stack->arr = SafeRealloc(stack->arr, stack->arr_size * sizeof(Poly));
}

Stack* StackCreate() {
    Stack *stack = SafeMalloc(sizeof(Stack));
    stack->arr = SafeMalloc(INIT_STACK_ARR_SIZE * sizeof(Poly));
    stack->arr_size = INIT_STACK_ARR_SIZE;
    stack->size = 0;
    return stack;
}

bool StackIsEmpty(Stack *stack) {
    return stack->size == 0;
}

bool StackUnderflow(Stack *stack, size_t size) {
    return stack->size < size;
}

void StackPush(Stack *stack, Poly *p) {
    if (stack->arr_size == stack->size)
        StackIncreaseSize(stack);
    stack->arr[stack->size++] = *p;
}

Poly* StackTop(Stack *stack) {
    assert(!StackIsEmpty(stack));
    return &stack->arr[stack->size - 1];
}

Poly* StackPrevTop(Stack *stack) {
    assert(!StackUnderflow(stack, 2));
    return &stack->arr[stack->size - 2];
}

Poly* StackPop(Stack *stack) {
    assert(!StackIsEmpty(stack));
    return &stack->arr[--stack->size];
}

void StackClear(Stack *stack) {
    while (!StackIsEmpty(stack)) {
        Poly *poly = StackPop(stack);
        PolyDestroy(poly);
    }
    free(stack->arr);
    free(stack);
}
