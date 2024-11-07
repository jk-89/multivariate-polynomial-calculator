# Multivariable Polynomial Calculator

This project implements a multivariable polynomial calculator in C, featuring:
* Basic operations like polynomial addition, subtraction, multiplication, etc.
* Polynomial composition.
* A command-line interface that allows interaction via commands like `ADD`, `MUL` etc.
* Various constructors for creating polynomial objects.

Documentation (in Polish) can be generated using `Doxygen`.

## Usage

First use
```
mkdir release
cd release
cmake ..
```
Then
* `make` creates an executable `poly`.
* `make test` creates an executable `poly_test` that tests the library.
* `make doc` creates documentation in `Doxygen` format.
