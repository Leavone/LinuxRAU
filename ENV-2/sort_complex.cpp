#include "sort_complex.h"

namespace complex {
    void sort_array_of_complex_numbers(ComplexNumber c_arr[], int n) {
        for (int i = 1; i < n; ++i) {
            ComplexNumber key = c_arr[i];
            int j = i - 1;

            while (j >= 0 && c_arr[j].abs() > key.abs()) {
                c_arr[j + 1] = c_arr[j];
                j = j - 1;
            }
            c_arr[j + 1] = key;
        }
    }

}
