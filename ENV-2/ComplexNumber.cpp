#include "ComplexNumber.h"
#include <cmath>

namespace complex {
    ComplexNumber::ComplexNumber() : real(0), imaginary(0) {}

    ComplexNumber::ComplexNumber(double re, double im) : real(re), imaginary(im) {}

    ComplexNumber operator+(const ComplexNumber &c1, const ComplexNumber &c2) {
        return ComplexNumber(c1.real + c2.real, c1.imaginary + c2.imaginary);
    }

    ComplexNumber operator-(const ComplexNumber &c1, const ComplexNumber &c2) {
        return ComplexNumber(c1.real - c2.real, c1.imaginary - c2.imaginary);
    }

    ComplexNumber operator*(const double &sc, const ComplexNumber &c2) {
        return ComplexNumber(sc * c2.real, sc * c2.imaginary);
    }

    std::ostream &operator<<(std::ostream &os, const ComplexNumber &c) {
        os << c.real << '+' << c.imaginary << 'i';
        return os;
    }

    double ComplexNumber::abs() {
        return std::sqrt(real * real + imaginary * imaginary);
    }

}