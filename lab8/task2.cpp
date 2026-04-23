#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int tag = 100;

    // Соседи по кольцу
    int right = (rank + 1) % size;
    int left = (rank - 1 + size) % size;

    // Данные для отправки
    double send_value = rank * 1.0;
    double recv_value = 0.0;

    // Буфер для MPI_Ibsend
    int buffer_size = sizeof(double) + MPI_BSEND_OVERHEAD;
    std::vector<char> buffer(buffer_size);

    MPI_Buffer_attach(buffer.data(), buffer_size);

    MPI_Request request;

    // Неблокирующая буферизованная отправка соседу справа
    MPI_Ibsend(&send_value, 1, MPI_DOUBLE, right, tag, MPI_COMM_WORLD, &request);

    // Блокирующий прием от соседа слева
    MPI_Recv(&recv_value, 1, MPI_DOUBLE, left, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Ждём завершения неблокирующей отправки
    MPI_Wait(&request, MPI_STATUS_IGNORE);

    std::cout << "Процесс " << rank
              << " отправил " << send_value
              << " процессу " << right
              << " и получил " << recv_value
              << " от процесса " << left
              << std::endl;

    // Отсоединяем буфер
    void* detached_buffer = nullptr;
    int detached_size = 0;
    MPI_Buffer_detach(&detached_buffer, &detached_size);

    MPI_Finalize();
    return 0;
}