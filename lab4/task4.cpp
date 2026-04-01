#include <iostream>
#include <omp.h>

int main() {
    int thread_count = 0;

#pragma omp parallel
    {
        int tid = omp_get_thread_num();

#pragma omp atomic
        thread_count++;

#pragma omp critical
        {
            std::cout << "Поток " << tid << " учтён" << std::endl;
        }
    }

    std::cout << "Общее количество потоков: "
              << thread_count << std::endl;

    return 0;
}