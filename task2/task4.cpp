#include <iostream>
#include <omp.h>

int main() {
    omp_set_num_threads(4);

#pragma omp parallel
    {
        int token = -1;        // локальная (private) переменная потока
        double alpha = 0.0;    // ещё одна локальная переменная

        // Один поток вычисляет "нужные" значения,
        // а copyprivate копирует их в private-переменные остальных потоков.
#pragma omp single copyprivate(token, alpha)
        {
            token = 42;
            alpha = 3.141592653589793;
            std::cout << "[single] computed token=" << token
                      << ", alpha=" << alpha
                      << " by thread " << omp_get_thread_num() << "\n";
        }
        // После single+copyprivate: у каждого потока token и alpha будут такими же.

#pragma omp barrier
        std::cout << "thread " << omp_get_thread_num()
                  << " sees token=" << token
                  << ", alpha=" << alpha << "\n";
    }

    return 0;
}