#include <iostream>
#include <omp.h>

int main() {

    omp_set_num_threads(4);

#pragma omp parallel
    {
        int token = -1;          // локальная переменная потока
        double alpha = 0.0;      // ещё одна локальная переменная

        // Один поток вычисляет значения,
        // а copyprivate копирует их в переменные остальных потоков
#pragma omp single copyprivate(token, alpha)
        {
            token = 42;
            alpha = 3.141592653589793;

#pragma omp critical
            {
                std::cout << "[single] Поток "
                          << omp_get_thread_num()
                          << " вычислил значения: token=" << token
                          << ", alpha=" << alpha
                          << std::endl;
            }
        }

#pragma omp barrier

#pragma omp critical
        {
            std::cout << "Поток " << omp_get_thread_num()
                      << " получил значения: token=" << token
                      << ", alpha=" << alpha
                      << std::endl;
        }
    }

    return 0;
}