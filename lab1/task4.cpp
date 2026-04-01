#include <iostream>
#include <omp.h>

int main() {
    // Принудительно задаём нужное количество потоков
    omp_set_num_threads(4);

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();

        std::cout << "Привет от потока " << thread_id
                  << " из " << total_threads << " потоков" << std::endl;
    }

    return 0;
}