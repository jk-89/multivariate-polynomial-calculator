/** @file
 * Implementacja modułu odpowiedzialnego za przetworzenie wczytanej linii.
 *
 * @author Jan Kwiatkowski
 */

#include "process_line.h"
#include "calc_functions.h"
#include "poly_stack.h"
#include "utilities.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/** Liczba komend. */
#define NUMBER_OF_COMMANDS 12

/**
 * Struktura przechowująca wskaźnik na funkcję oraz jej nazwę.
 * Umożliwia łatwe przetwarzanie komend kalkulatora.
 */
typedef struct {
    bool (*function)(Stack*); ///< funkcja
    char const *name; ///< nazwa funkcji
} Command;

/**
 * Tablica zawierająca większość funkcji udostępnianych przez kalkulator.
 * Zawiera funkcje nieprzyjmujące dodatkowego argumentu (poza stosem).
 */
const Command FUNCTION_NAMES[NUMBER_OF_COMMANDS] = {
        {Zero, "ZERO"},
        {IsCoeff, "IS_COEFF"},
        {IsZero, "IS_ZERO"},
        {Clone, "CLONE"},
        {Add, "ADD"},
        {Mul, "MUL"},
        {Neg, "NEG"},
        {Sub, "SUB"},
        {IsEq, "IS_EQ"},
        {Deg, "DEG"},
        {Print, "PRINT"},
        {Pop, "POP"},
};

/** Nazwa komendy @ref DegBy(Stack *stack, size_t idx). */
const char *DEG_BY_COMMAND = "DEG_BY";
/** Nazwa komendy @ref At(Stack *stack, poly_coeff_t x). */
const char *AT_COMMAND = "AT";
/** Nazwa komendy @ref Compose(Stack *stack, size_t k). */
const char *COMPOSE_COMMAND = "COMPOSE";

/** Znaki dopuszczalne w wielomianie. */
const char *ALLOWED_POLY_CHARS = "0123456789-+,()";
/** Znaki dopuszczalne w liczbie. */
const char *ALLOWED_NUMBER_CHARS = "0123456789-";

/** Maksymalna wartość @ref poly_exp_t. */
const long MAX_POLY_EXP_T = 2147483647;

/** Komendy wykonywane są w systemie dziesiętnym. */
const int BASE = 10;

/**
 * Wypisuje informację o błędnej nazwie komendy w danym wierszu.
 * @param[in] index : numer wiersza
 */
static void WrongCommandError(const size_t *index) {
    fprintf(stderr, "ERROR %zu WRONG COMMAND\n", *index);
}

/**
 * Wypisuje informację o błędnie zapisanym wielomianie w danym wierszu.
 * @param[in] index : numer wiersza
 */
static void WrongPolyError(const size_t *index) {
    fprintf(stderr, "ERROR %zu WRONG POLY\n", *index);
}

/**
 * Wypisuje informację o błędnym argumencie funkcji.
 * @ref At(Stack *stack, poly_coeff_t x) w danym wierszu.
 * @param[in] index : numer wiersza
 */
static void AtWrongValueError(const size_t *index) {
    fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", *index);
}

/**
 * Wypisuje informację o błędnym argumencie funkcji.
 * @ref DegBy(Stack *stack, size_t idx) w danym wierszu.
 * @param[in] index : numer wiersza
 */
static void DegByWrongVariableError(const size_t *index) {
    fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", *index);
}

/**
 * Wypisuje informację o zbyt małej liczbie wielomianów na stosie w danym
 * wierszu.
 * Funkcja jest wykorzystywana, gdy na stosie jest zbyt mało wielomianów, by
 * wykonać daną komendę.
 * @param[in] index : numer wiersza
 */
static void StackUnderflowError(const size_t *index) {
    fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", *index);
}

/**
 * Wypisuje informację o błędnym argumencie funkcji.
 * @param[in] index : numer wiersza
 */
static void ComposeError(const size_t *index) {
    fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", *index);
}

/**
 * Tworzy 'zerowy' jednomian.
 * Funkcja jest potrzebna by zwrócić jakikolwiek jednomian w przypadku błędnego
 * wielomianu w funkcji @ref CreateMono(char *begin, char *end, bool *is_poly).
 * @return Jednomian składający się z wielomianu zerowego podniesionego do
 * zerowej potęgi.
 */
static Mono MonoZero() {
    return (Mono) {.p = PolyZero(), .exp = 0};
}

/**
 * Sprawdza czy dany znak jest literą (małą bądź wielką).
 * @param[in] ch : znak
 * @return czy dany znak jest literą
 */
static bool IsLetter(char ch) {
    int ascii = (int) ch;
    return (ascii >= 'a' && ascii <= 'z') || (ascii >= 'A' && ascii <= 'Z');
}

/**
 * Sprawdza czy napis zawiera tylko i wyłącznie znaki dopuszczalne
 * w wielomianie.
 * @param[in] characters : liczba znaków
 * @param[in] input : ciąg znaków
 * @return czy napis zawiera tylko i wyłącznie znaki dopuszczalne w wielomianie
 */
static bool CheckPolyChars(const size_t *characters, const char *input) {
    for (size_t i = 0; i < *characters; i++) {
        char *ch = strchr(ALLOWED_POLY_CHARS, input[i]);
        if (ch == NULL)
            return false;
    }
    return true;
}

/**
 * Sprawdza czy napis zawiera tylko i wyłącznie znaki dopuszczalne w liczbie.
 * @param[in] input : ciąg znaków
 * @return czy napis zawiera tylko i wyłącznie znaki dopuszczalne w liczbie
 */
static bool CheckNumberChars(char *input) {
    for (char *i = input; *i != '\0'; i++) {
        char *ch = strchr(ALLOWED_NUMBER_CHARS, *i);
        if (ch == NULL)
            return false;
    }
    return true;
}

/**
 * Z ciągu znaków tworzy liczbę typu @ref poly_exp_t (o ile to możliwe).
 * @param[in] begin : początek ciągu znaków
 * @param[in,out] endptr : miejsce, do którego funkcja strtoll() przetworzyła
 * ciąg znaków
 * @return liczba typu @ref poly_exp_t
 */
static poly_exp_t StrToPolyExp(char *begin, char **endptr) {
    long val = strtoll(begin, endptr, BASE);
    if (val > MAX_POLY_EXP_T || val < 0) {
        errno = ERANGE;
        return 0;
    }
    return (poly_exp_t) (val);
}

/**
 * Sprawdza czy komenda wymagająca podania argumentów ma po nazwie spację oraz
 * co najmniej jeden znak i nie zawiera znaku '\0'.
 * @param[in] read_characters : liczba znaków w ciągu
 * @param[in] length : długość komendy
 * @param[in] input : ciąg znaków
 * @param[in] line_length : liczba znaków w wierszu
 * @return czy komenda spełnia opisane warunki
 */
static bool CheckArguments(const size_t *read_characters, const size_t *length,
                           const char *input, const size_t *line_length) {
    return *read_characters >= *length + 2 && input[*length] == ' ' &&
    *read_characters == *line_length;
}

/**
 * Przetwarza wiersz zawierający na początku komendę wymagająca podania jako
 * argumentu zmiennej typu size_t.
 * Sprawdza poprawność argumentów, jeśli są poprawne wykonuje tą komendę.
 * @param[in] index : numer wiersza
 * @param[in] read_characters : długość ciągu znaków
 * @param[in] input : ciąg znaków
 * @param[in,out] stack : stos
 * @param[in] length : długość wiersza
 * @param[in] command_name : nazwa komendy
 * @param[in] error : funkcja, którą należy wywołać w przypadku niepoprawnego
 * argumentu w komendzie
 * @param[in] function : funkcja, którą należy wywołać jeśli argument jest
 * poprawny
 */
static void Size_TCommand(const size_t *index, const size_t *read_characters,
                       char *input, Stack *stack, const size_t *length,
                       const char *command_name, void (*error)(const size_t*),
                       bool (*function)(Stack*, size_t)) {
    size_t command_length = strlen(command_name);

    if (!CheckArguments(read_characters, &command_length, input, length)) {
        if ((*read_characters != command_length && input[command_length] != ' ')
        || (*read_characters == command_length && *read_characters != *length))
            WrongCommandError(index);
        else
            error(index);
        return;
    }

    // na początku liczby nie może być '+' ani '-'
    if (input[command_length + 1] == '+' || input[command_length + 1] == '-' ||
        !CheckNumberChars(input + command_length + 1)) {
        error(index);
        return;
    }

    char *endptr;
    size_t value = strtoull(&input[command_length + 1], &endptr, BASE);

    // sprawdzenie czy na wejściu podano liczbę z poprawnego zakresu oraz czy
    // w linii nie ma dodatkowych znaków
    if (CheckErrno() && *endptr == '\0') {
        if (!function(stack, value))
            StackUnderflowError(index);
    }
    else {
        error(index);
    }
}

/**
 * Przetwarza wiersz zawierający na początku komendę
 * @ref At(Stack *stack, poly_coeff_t x).
 * Sprawdza poprawność argumentów, jeśli są poprawne wykonuje tą komendę.
 * @param[in] index : numer wiersza
 * @param[in] read_characters : długość ciągu znaków
 * @param[in] input : ciąg znaków
 * @param[in,out] stack : stos
 * @param[in] length : długość wiersza
 */
static void ProcessAt(const size_t *index, const size_t *read_characters,
                      char *input, Stack *stack, const size_t *length) {
    size_t at_length = strlen(AT_COMMAND);

    if (!CheckArguments(read_characters, &at_length, input, length)) {
        if ((*read_characters != at_length && input[at_length] != ' ')
        || (*read_characters == at_length && *read_characters != *length))
            WrongCommandError(index);
        else
            AtWrongValueError(index);
        return;
    }

    // na początku liczby nie może być '+'
    if (input[at_length + 1] == '+' ||
    !CheckNumberChars(input + at_length + 1)) {
        AtWrongValueError(index);
        return;
    }

    char *endptr;
    poly_coeff_t coeff = strtoll(&input[at_length + 1], &endptr, BASE);

    // sprawdzenie czy na wejściu podano liczbę z poprawnego zakresu oraz czy
    // w linii nie ma dodatkowych znaków
    if (CheckErrno() && *endptr == '\0') {
        if (!At(stack, coeff))
            StackUnderflowError(index);
    }
    else {
        AtWrongValueError(index);
    }
}

/**
 * Przetwarza wiersz zawierający nazwę komendy.
 * W przypadku błędnej nazwy wypisuje stosowny komunikat.
 * @param[in] index : numer wiersza
 * @param[in] read_characters : długość ciągu znaków
 * @param[in] input : ciąg znaków
 * @param[in,out] stack : stos
 * @param[in] length : długość wiersza
 */
static void ProcessCommand(const size_t *index, const size_t *read_characters,
                           char *input, Stack *stack, const size_t *length) {
    size_t deg_by_length = strlen(DEG_BY_COMMAND);
    size_t at_length = strlen(AT_COMMAND);
    size_t compose_length = strlen(COMPOSE_COMMAND);

    // należy osobno sprawdzić DegBy() oraz At()
    if (*read_characters >= deg_by_length &&
    strncmp(DEG_BY_COMMAND, input, deg_by_length) == 0) {
        Size_TCommand(index, read_characters, input, stack, length,
                      DEG_BY_COMMAND, &DegByWrongVariableError, &DegBy);
        return;
    }
    else if (*read_characters >= compose_length &&
    strncmp(COMPOSE_COMMAND, input, compose_length) == 0) {
        Size_TCommand(index, read_characters, input, stack, length,
                      COMPOSE_COMMAND, &ComposeError, &Compose);
        return;
    }
    else if (*read_characters >= at_length &&
    strncmp(AT_COMMAND, input, at_length) == 0) {
        ProcessAt(index, read_characters, input, stack, length);
        return;
    }

    // występuje znak '\0'
    if (*length != *read_characters) {
        WrongCommandError(index);
        return;
    }

    for (size_t i = 0; i < (size_t) NUMBER_OF_COMMANDS; i++) {
        size_t name_length = strlen(FUNCTION_NAMES[i].name);
        if (*read_characters == name_length &&
        strncmp(FUNCTION_NAMES[i].name, input, name_length) == 0) {
            if (!(*FUNCTION_NAMES[i].function)(stack))
                StackUnderflowError(index);
            return;
        }
    }

    WrongCommandError(index);
}

static Poly CreatePoly(char *begin, char *end, bool *is_poly);

/**
 * Z ciągu znaków próbuje utworzyć jednomian.
 * @param[in] begin : początek ciągu znaków
 * @param[in] end : pierwszy znak poza ciągiem znaków
 * @param[in,out] is_poly : czy przetwarzany ciąg znaków ma szansę być poprawnym
 * wielomianem
 * @return jednomian
 */
static Mono CreateMono(char *begin, char *end, bool *is_poly) {
    if (begin >= end) {
        *is_poly = false;
        return MonoZero();
    }

    Mono mono = MonoZero();
    char *i = end - 1;

    // jednomian ma postać (wielomian, wykładnik)
    while (true) {
        if (*i == ',') {
            if (i + 1 == end || *(i + 1) == '+') {
                *is_poly = false;
                break;
            }
            mono.p = CreatePoly(begin, i, is_poly);
            char *endptr = NULL;
            mono.exp = StrToPolyExp(i + 1, &endptr);
            if (!CheckErrno() || endptr != end)
                *is_poly = false;
            break;
        }

        if (i == begin) {
            *is_poly = false;
            break;
        }
        i--;
    }

    if (!*is_poly) {
        PolyDestroy(&mono.p);
        return MonoZero();
    }

    return mono;
}

/**
 * Dla danego ciągu znaków tworzących wielomian znajduje liczbę jednomianów,
 * które on zawiera.
 * Jeśli liczba nawiasów otwierających różni się od liczby nawiasów zamykających
 * lub nie występują żadne nawiasy lub na pewnym prefiksie jest więcej nawiasów
 * zamykających niż otwierających, to wielomian na pewno nie jest poprawny.
 * @param[in] begin : początek ciągu znaków
 * @param[in] end : pierwszy znak poza ciągiem znaków
 * @param[in,out] correct : czy wielomian jest poprawny
 * @return liczba jednomianów
 */
static size_t CountMonosAndCheck(char *begin, char *end, bool *correct) {
    int brackets = 0;
    size_t number_of_monos = 0;

    for (char *i = begin; i != end; i++) {
        if (*i == '(')
            brackets++;
        if (*i == ')') {
            if (brackets == 0) {
                *correct = false;
                break;
            }
            brackets--;
            if (brackets == 0)
                number_of_monos++;
        }
    }

    if (number_of_monos == 0 || brackets != 0)
        *correct = false;

    return number_of_monos;
}

/**
 * Czyści zawartość tablicy jednomianów.
 * @param[in,out] monos : tablica jednomianów
 * @param[in] size : rozmiar tablicy
 */
static void ClearMonosArray(Mono *monos, size_t size) {
    for (size_t i = 0; i < size; i++)
        MonoDestroy(&monos[i]);
}

/**
 * Z ciągu znaków próbuje utworzyć wielomian.
 * @param[in] begin : początek ciągu znaków
 * @param[in] end : pierwszy znak poza ciągiem znaków
 * @param[in,out] is_poly : czy przetwarzany ciąg znaków ma szansę być poprawnym
 * wielomianem
 * @return wielomian
 */
static Poly CreatePoly(char *begin, char *end, bool *is_poly) {
    if (begin >= end) {
        *is_poly = false;
        return PolyZero();
    }

    // czy wielomian jest wielomianem stałym
    if (*begin != '+') {
        char *endptr;
        poly_coeff_t coeff = strtoll(begin, &endptr, BASE);
        if (CheckErrno() && endptr == end) {
            return PolyFromCoeff(coeff);
        }
    }

    bool correct = true;
    size_t number_of_monos = CountMonosAndCheck(begin, end, &correct);
    if (!correct) {
        *is_poly = false;
        return PolyZero();
    }

    Mono *monos = SafeMalloc(sizeof(Mono) * number_of_monos);
    int brackets = 0;
    size_t processed_monos = 0;
    char *begptr = NULL, *endptr = NULL;
    bool processing_mono = false;
    // wielomian jest postaci (jednomian)+(jednomian)+...+(jednomian)
    for (char *i = begin; i != end; i++) {
        if (!processing_mono) {
            if (*i != '(') {
                *is_poly = false;
                break;
            }
            else {
                begptr = i + 1;
                processing_mono = true;
            }
        }
        if (*i == '(')
            brackets++;

        if (*i == ')') {
            brackets--;
            if (brackets == 0) {
                endptr = i;
                monos[processed_monos++] = CreateMono(begptr, endptr, is_poly);
                if (!*is_poly)
                    break;
                processing_mono = false;
                if (i + 1 != end) {
                    i++;
                    if (*i != '+') {
                        *is_poly = false;
                        break;
                    }
                }
            }
        }

        if (i + 1 == end && *i != ')')
            *is_poly = false;
    }

    if (!*is_poly) {
        ClearMonosArray(monos, processed_monos);
        free(monos);
        return PolyZero();
    }

    Poly poly = PolyOwnMonos(processed_monos, monos);
    return poly;
}

/**
 * Przetwarza wiersz zawierający wielomian.
 * W przypadku niepoprawnego wielomianu wypisuje stosowny komunikat.
 * @param[in] index : numer wiersza
 * @param[in] read_characters : długość wiersza
 * @param[in] input : ciąg znaków
 * @param[in,out] stack : stos
 */
static void ProcessPoly(const size_t *index, const size_t *read_characters,
                        char *input, Stack *stack) {
    if (!CheckPolyChars(read_characters, input)) {
        WrongPolyError(index);
        return;
    }

    bool is_poly = true;
    Poly poly = CreatePoly(input, input + *read_characters, &is_poly);
    if (!is_poly) {
        WrongPolyError(index);
        return;
    }
    StackPush(stack, &poly);
}

bool CheckErrno() {
    bool ret = errno == 0;
    errno = 0;
    return ret;
}

void ProcessInput(const size_t *index, size_t *read_characters,
                  char *input, Stack *stack) {
    // sprawdzenie czy wiersz jest komentarzem
    if (*read_characters != 0 && input[0] == '#')
        return;

    // sprawdzenie czy wiersz zawiera znak '\0' w środku
    size_t length = strlen(input);
    if (*read_characters != length) {
        if (*read_characters != 0 && IsLetter(input[0]))
            ProcessCommand(index, read_characters, input, stack, &length);
        else
            WrongPolyError(index);
        return;
    }

    // pusta linia
    if (*read_characters == 0)
        return;

    // znak nowej linii nie jest brany pod uwagę
    if (input[*read_characters - 1] == '\n') {
        *read_characters = *read_characters - 1;
        input[*read_characters] = '\0';
        length--;
    }

    // pusta linia
    if (*read_characters == 0)
        return;

    if (IsLetter(input[0]))
        ProcessCommand(index, read_characters, input, stack, &length);
    else
        ProcessPoly(index, read_characters, input, stack);
}
