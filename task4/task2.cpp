#include <iostream>
#include <omp.h>

int main() {
    const int N = 10;

    std::cout << "Начало параллельного цикла:\n";

#pragma omp parallel for ordered
    for (int i = 0; i < N; i++) {
        int tid = omp_get_thread_num();

        // Параллельная часть: может выполняться в любом порядке
        std::cout << "Поток " << tid
                  << " подготовил итерацию i = " << i << '\n';

        // Упорядоченная часть: выполняется строго по i = 0, 1, 2, ...
#pragma omp ordered
        {
            std::cout << "Упорядоченный вывод: i = " << i
                      << ", поток " << tid << '\n';
        }
    }

    return 0;
}