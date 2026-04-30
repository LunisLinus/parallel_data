#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0, size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double value;

    // Процесс 0 обновляет значение переменной
    if (rank == 0) {
        value = 10.5;
        std::cout << "Процесс 0 обновил значение: " << value << std::endl;
    }

    // Широковещательная рассылка значения всем процессам
    MPI_Bcast(&value, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Каждый процесс прибавляет свой номер
    double local_result = value + rank;

    std::cout << "Процесс " << rank
              << " получил " << value
              << " и вычислил " << local_result
              << std::endl;

    // Буфер для сбора результатов на процессе 0
    std::vector<double> gathered_data;
    if (rank == 0) {
        gathered_data.resize(size);
    }

    // Сбор результатов от всех процессов в процессе 0
    MPI_Gather(&local_result, 1, MPI_DOUBLE,
               gathered_data.data(), 1, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    // Вывод результатов на процессе 0
    if (rank == 0) {
        std::cout << "\nПроцесс 0 собрал результаты:\n";
        for (int i = 0; i < size; ++i) {
            std::cout << "От процесса " << i
                      << ": " << gathered_data[i] << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}