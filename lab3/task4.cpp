#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const int N = 20; // для наглядности можно поставить маленькое число
    std::vector<int> A(N), B(N), C(N);

    // Инициализация векторов
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = i * 10;
    }

    std::cout << "Сложение векторов с использованием omp for:\n";

#pragma omp parallel
    {
        int tid = omp_get_thread_num();

#pragma omp for
        for (int i = 0; i < N; i++) {
            C[i] = A[i] + B[i];

#pragma omp critical
            {
                std::cout << "Поток " << tid
                          << " обработал итерацию i = " << i
                          << ", C[" << i << "] = " << C[i] << '\n';
            }
        }
    }

    std::cout << "\nРезультирующий вектор C:\n";
    for (int i = 0; i < N; i++) {
        std::cout << C[i] << " ";
    }
    std::cout << '\n';

    return 0;
}