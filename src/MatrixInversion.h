#ifndef MATRIX_INVERSION_H
#define MATRIX_INVERSION_H

#include <vector>

template <typename T>
class MatrixInversion {
public:
    static std::vector<std::vector<T>> gaussianElimination(std::vector<std::vector<T>> a) {
        std::vector<std::vector<T>> b(a.size(), std::vector<T>(a.size(), (T) 0));
        for (int i = 0; i < a.size(); ++i) {
            b[i][i] = (T) 1;
        }
        for (int i = 0; i < a.size(); ++i) {
            int l = i;
            while (l < a.size() && a[l][i] == (T) 0) {
                ++l;
            }
            if (l < a.size()) {
                if (l != i) {
                    std::swap(a[i], a[l]);
                }
                if (a[i][i] != (T) 0) {
                    for (int j = i + 1; j < a.size(); ++j) {
                        if (a[j][i] != (T) 0) {
                            T t = a[j][i] / a[i][i];
                            for (int k = 0; k < i; ++k) {
                                b[j][k] -= t * b[i][k];
                            }
                            for (int k = i; k < a.size(); ++k) {
                                a[j][k] -= t * a[i][k];
                                b[j][k] -= t * b[i][k];
                            }
                        }
                    }
                }
            }
        }
        for (int i = a.size() - 1; i >= 0; --i) {
            for (int j = i + 1; j < a.size(); ++j) {
                for (int k = 0; k < a.size(); ++k) {
                    b[i][k] -= b[j][k] * a[i][j];
                }
            }
            if (a[i][i] != (T) 0) {
                for (int j = 0; j < a.size(); ++j) {
                    b[i][j] /= a[i][i];
                }
            } else {
                // no solutions or infinitely many solutions
            }
        }
        return b;
    }
};


#endif //MATRIX_INVERSION_H
