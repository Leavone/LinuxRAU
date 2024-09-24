#pragma once

#include <iostream>

namespace complex {
    class ComplexNumber {
    private:
        double real;
        double imaginary;
    public:
        ComplexNumber();

        ComplexNumber(double re, double im);

        double abs();

        friend ComplexNumber operator+(const ComplexNumber &c1, const ComplexNumber &c2);

        friend ComplexNumber operator-(const ComplexNumber &c1, const ComplexNumber &c2);

        friend ComplexNumber operator*(const double &sc, const ComplexNumber &c2);

        friend std::ostream &operator<<(std::ostream &os, const ComplexNumber &c);
    };

}