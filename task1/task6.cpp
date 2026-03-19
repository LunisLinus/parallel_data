#include <iostream>
#include <omp.h>

int main() {
    int thread_count = 0;

#pragma omp parallel reduction(+:thread_count)
    {
        // Каждый поток добавляет 1
        thread_count += 1;
    }

    std::cout << "Фактическое число порожденных потоков: "
              << thread_count << std::endl;

    return 0;
}