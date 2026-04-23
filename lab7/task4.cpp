#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            std::cerr << "Нужно минимум 2 процесса.\n";
        }
        MPI_Finalize();
        return 1;
    }

    const int tag1 = 200;
    const int tag2 = 201;

    if (rank == 0) {
        double value = 2.718281828;

        // Блокирующая отправка процессу 1
        MPI_Send(&value, 1, MPI_DOUBLE, 1, tag1, MPI_COMM_WORLD);
        std::cout << "Процесс 0 отправил число: " << value << std::endl;

        // Ожидание ответа
        double reply = 0.0;
        MPI_Recv(&reply, 1, MPI_DOUBLE, 1, tag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::cout << "Процесс 0 получил ответ: " << reply << std::endl;
    }
    else if (rank == 1) {
        double received = 0.0;

        // Блокирующий прием
        MPI_Recv(&received, 1, MPI_DOUBLE, 0, tag1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Процесс 1 получил число: " << received << std::endl;

        // Формируем ответ
        double reply = received * 2.0;

        // Блокирующая отправка ответа
        MPI_Send(&reply, 1, MPI_DOUBLE, 0, tag2, MPI_COMM_WORLD);
        std::cout << "Процесс 1 отправил ответ: " << reply << std::endl;
    }

    MPI_Finalize();
    return 0;
}