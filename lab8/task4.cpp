#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int left  = (rank - 1 + size) % size;
    int right = (rank + 1) % size;

    int send_value = rank;
    int recv_value = -1;

    MPI_Sendrecv(
        &send_value, 1, MPI_INT, right, 0,   // отправка вправо
        &recv_value, 1, MPI_INT, left,  0,   // приём слева
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );

    std::cout << "Process " << rank
              << " sent " << send_value << " to " << right
              << ", received " << recv_value << " from " << left
              << std::endl;

    MPI_Finalize();
    return 0;
}