#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0, size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int tag = 100;

    if (size < 2) {
        if (rank == 0) {
            std::cerr << "Для работы программы нужно минимум 2 процесса.\n";
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // MASTER
        int slaves = size - 1;

        // По одному буферу на каждого slave
        std::vector<int> recv_data(slaves, 0);

        // Массив запросов на неблокирующий прием
        std::vector<MPI_Request> requests(slaves);

        // Размещаем Irecv от каждого ведомого процесса
        for (int i = 0; i < slaves; ++i) {
            int source = i + 1; // процессы 1..size-1
            MPI_Irecv(&recv_data[i], 1, MPI_INT, source, tag, MPI_COMM_WORLD, &requests[i]);
        }

        int completed_total = 0;

        while (completed_total < slaves) {
            int outcount = 0;
            std::vector<int> indices(slaves);
            std::vector<MPI_Status> statuses(slaves);

            MPI_Waitsome(
                slaves,
                requests.data(),
                &outcount,
                indices.data(),
                statuses.data()
            );

            // MPI_Waitsome может вернуть MPI_UNDEFINED, если активных запросов не осталось
            if (outcount == MPI_UNDEFINED) {
                break;
            }

            for (int i = 0; i < outcount; ++i) {
                int idx = indices[i];           // индекс в массиве requests / recv_data
                int source = idx + 1;           // соответствующий slave
                int value = recv_data[idx];

                std::cout << "Master получил сообщение от процесса "
                          << source << ": " << value << std::endl;
            }

            completed_total += outcount;
        }
    } else {
        // SLAVE
        int value = rank * 10;

        MPI_Send(&value, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

        std::cout << "Slave " << rank
                  << " отправил сообщение master: "
                  << value << std::endl;
    }

    MPI_Finalize();
    return 0;
}