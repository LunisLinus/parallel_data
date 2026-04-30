#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 1. Имя процессора (узла)
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len = 0;
    MPI_Get_processor_name(processor_name, &name_len);

    // 2. Разрешение таймера MPI
    double tick = MPI_Wtick();

    // 3. Среднее время, требуемое на замер времени
    // Будем многократно вызывать MPI_Wtime() и делить общее время на число вызовов.
    const int N = 1000000;

    MPI_Barrier(MPI_COMM_WORLD); // чтобы процессы стартовали примерно одновременно

    double t1 = MPI_Wtime();
    for (int i = 0; i < N; ++i)
    {
        volatile double tmp = MPI_Wtime();
        (void)tmp;
    }
    double t2 = MPI_Wtime();

    double avg_measure_time = (t2 - t1) / N;

    // Печатаем по порядку, чтобы вывод не перемешивался
    for (int p = 0; p < size; ++p)
    {
        if (p == rank)
        {
            std::cout << "Process " << rank << " of " << size << "\n";
            std::cout << "Processor name: " << processor_name << "\n";
            std::cout << std::setprecision(15);
            std::cout << "MPI timer resolution (MPI_Wtick): " << tick << " sec\n";
            std::cout << "Average time for one MPI_Wtime call: "
                      << avg_measure_time << " sec\n";
            std::cout << "-----------------------------\n";
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}