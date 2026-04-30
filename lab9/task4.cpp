#include <mpi.h>
#include <iostream>
#include <vector>

// Пользовательская операция: поэлементная сумма по модулю 8
void mod8_sum(void* invec, void* inoutvec, int* len, MPI_Datatype* datatype) {
    int* in = static_cast<int*>(invec);
    int* inout = static_cast<int*>(inoutvec);

    for (int i = 0; i < *len; ++i) {
        inout[i] = (inout[i] + in[i]) % 8;
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0, size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = 5;

    // Локальный вектор каждого процесса
    std::vector<int> local(n);
    for (int i = 0; i < n; ++i) {
        local[i] = rank + i;
    }

    std::vector<int> result(n, 0);

    // Создание пользовательской коммутативной операции
    MPI_Op mod8_op;
    MPI_Op_create(mod8_sum, 1, &mod8_op);

    // Глобальная операция свёртки
    MPI_Reduce(local.data(), result.data(), n, MPI_INT, mod8_op, 0, MPI_COMM_WORLD);

    // Вывод локальных данных
    std::cout << "Процесс " << rank << ": ";
    for (int x : local) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // Вывод результата на процессе 0
    if (rank == 0) {
        std::cout << "\nРезультат суммирования по модулю 8:\n";
        for (int x : result) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }

    MPI_Op_free(&mod8_op);
    MPI_Finalize();
    return 0;
}