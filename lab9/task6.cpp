#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    MPI_Comm old_comm = MPI_COMM_WORLD;
    MPI_Comm graph_comm;

    int world_rank, world_size;
    MPI_Comm_rank(old_comm, &world_rank);
    MPI_Comm_size(old_comm, &world_size);

    // Описание графа "звезда":
    // вершина 0 соединена со всеми вершинами 1..world_size-1,
    // каждая вершина i>0 соединена только с вершиной 0.
    //
    // Формат для MPI_Graph_create:
    // indx[i]  - индекс конца списка соседей вершины i в массиве edges
    // edges[]  - объединённые списки соседей всех вершин подряд

    std::vector<int> indx(world_size);
    std::vector<int> edges;

    if (world_size == 1) {
        // Частный случай: один процесс, соседей нет
        indx[0] = 0;
    } else {
        // Соседи вершины 0: 1, 2, ..., world_size-1
        for (int i = 1; i < world_size; ++i) {
            edges.push_back(i);
        }
        indx[0] = world_size - 1;

        // Для каждой вершины i>0 единственный сосед — 0
        for (int i = 1; i < world_size; ++i) {
            edges.push_back(0);
            indx[i] = indx[i - 1] + 1;
        }
    }

    // Разрешаем перенумерацию процессов
    int reorder = 1;

    MPI_Graph_create(old_comm, world_size, indx.data(), edges.data(),
                     reorder, &graph_comm);

    if (graph_comm == MPI_COMM_NULL) {
        std::cerr << "Process " << world_rank
                  << ": graph communicator was not created\n";
        MPI_Finalize();
        return 1;
    }

    int graph_rank, graph_size;
    MPI_Comm_rank(graph_comm, &graph_rank);
    MPI_Comm_size(graph_comm, &graph_size);

    // Узнаём число соседей процесса в графе
    int nneighbors = 0;
    MPI_Graph_neighbors_count(graph_comm, graph_rank, &nneighbors);

    std::vector<int> neighbors(nneighbors);
    if (nneighbors > 0) {
        MPI_Graph_neighbors(graph_comm, graph_rank, nneighbors, neighbors.data());
    }

    // Печать информации о новой нумерации и соседях
    MPI_Barrier(graph_comm);
    for (int p = 0; p < graph_size; ++p) {
        if (p == graph_rank) {
            std::cout << "World rank = " << world_rank
                      << ", Graph rank = " << graph_rank
                      << ", Neighbors: ";
            if (nneighbors == 0) {
                std::cout << "none";
            } else {
                for (int i = 0; i < nneighbors; ++i) {
                    std::cout << neighbors[i] << " ";
                }
            }
            std::cout << std::endl;
        }
        MPI_Barrier(graph_comm);
    }

    // Обмен сообщениями только по топологии графа:
    // ненулевые процессы отправляют значение нулевому процессу,
    // нулевой процесс принимает от всех соседей и отправляет ответ.
    if (graph_rank != 0) {
        int send_value = graph_rank;
        int recv_value = -1;

        MPI_Send(&send_value, 1, MPI_INT, 0, 100, graph_comm);
        MPI_Recv(&recv_value, 1, MPI_INT, 0, 200, graph_comm, MPI_STATUS_IGNORE);

        std::cout << "Graph rank " << graph_rank
                  << " sent " << send_value
                  << " to 0 and received " << recv_value
                  << " from 0" << std::endl;
    } else {
        for (int i = 0; i < nneighbors; ++i) {
            int src = neighbors[i];
            int value = -1;

            MPI_Recv(&value, 1, MPI_INT, src, 100, graph_comm, MPI_STATUS_IGNORE);
            std::cout << "Graph rank 0 received " << value
                      << " from " << src << std::endl;

            int reply = value * 10;
            MPI_Send(&reply, 1, MPI_INT, src, 200, graph_comm);
        }
    }

    MPI_Comm_free(&graph_comm);
    MPI_Finalize();
    return 0;
}