#include <iostream>
#include <omp.h>

int main() {

    int x = 10;

    std::cout << "До параллельной области x = " << x << std::endl;

#pragma omp parallel private(x)
    {
        int id = omp_get_thread_num();

        x = id * 5;

#pragma omp critical
        {
            std::cout << "Поток " << id
                      << " изменил свой private x = "
                      << x << std::endl;
        }
    }

    std::cout << "После параллельной области x = " << x << std::endl;

    return 0;
}