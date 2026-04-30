#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            std::cerr << "Для выполнения программы нужно минимум 2 процесса.\n";
        }
        MPI_Finalize();
        return 1;
    }

    const int tag = 123;

    if (rank == 0) {
        // Сообщение заранее неизвестной длины
        std::vector<int> data = {10, 20, 30, 40, 50, 60, 70};

        MPI_Send(data.data(), data.size(), MPI_INT, 1, tag, MPI_COMM_WORLD);

        std::cout << "Процесс 0 отправил сообщение длины "
                  << data.size() << std::endl;
    }
    else if (rank == 1) {
        MPI_Status status;

        // Сначала "подсматриваем" сообщение
        MPI_Probe(0, tag, MPI_COMM_WORLD, &status);

        int count;
        MPI_Get_count(&status, MPI_INT, &count);

        std::cout << "Процесс 1 определил длину сообщения: "
                  << count << std::endl;

        // Выделяем память нужного размера
        std::vector<int> received(count);

        // Теперь принимаем сообщение
        MPI_Recv(received.data(), count, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::cout << "Процесс 1 получил данные: ";
        for (int x : received) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}