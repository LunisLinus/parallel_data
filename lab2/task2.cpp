#include <iostream>
#include <omp.h>

int main() {

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

#pragma omp single
        {
            std::cout << "Сообщение выводит поток: "
                      << thread_id << std::endl;
        }
    }

    return 0;
}