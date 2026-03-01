#include <iostream>
#include <omp.h>

int main() {
    double t0 = omp_get_wtime();   // начальное время (сек)
    
    // Небольшая работа, чтобы время отличалось от t0
    volatile double x = 0.0;
    for (long long i = 0; i < 200000000LL; ++i) {
        x += i * 1e-12;
    }

    double t1 = omp_get_wtime();   // конечное время (сек)
    double tick = omp_get_wtick(); // точность таймера (сек)

    std::cout << "Start time (s): " << t0 << "\n";
    std::cout << "End time   (s): " << t1 << "\n";
    std::cout << "Elapsed    (s): " << (t1 - t0) << "\n";
    std::cout << "Timer tick (s): " << tick << "\n";

    return 0;
}