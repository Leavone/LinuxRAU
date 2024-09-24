#include <iostream>
#include "sort_complex.h"

int main() {
    complex::ComplexNumber c1 = complex::ComplexNumber(1, 0);
    complex::ComplexNumber c2 = complex::ComplexNumber(2, 0);
    complex::ComplexNumber c3 = complex::ComplexNumber(3, 0);
    complex::ComplexNumber c4 = complex::ComplexNumber(4, 0);
    complex::ComplexNumber c5 = complex::ComplexNumber(5, 0);
    complex::ComplexNumber c6 = complex::ComplexNumber(6, 0);
    complex::ComplexNumber c7 = complex::ComplexNumber(7, 0);
    complex::ComplexNumber c_arr[7] = {c5, c7, c3, c1, c2, c4, c6};
    for (int i = 0; i < 7; ++i) {
        std::cout << c_arr[i] << ' ';
    }
    std::cout << '\n';
    complex::sort_array_of_complex_numbers(c_arr, 7);
    for (int i = 0; i < 7; ++i) {
        std::cout << c_arr[i] << ' ';
    }
    return 0;
}
