#include <mpi.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdlib>

#define TAG_WORK 1
#define TAG_STOP 2
#define TAG_RESULT 3

double func(double x, double y, double z)
{
    return 1.0 / (2.0 + exp(-x * x) * sin(y * y + z * z));
}

double compute_range(int N, int i_begin, int i_end)
{
    double h = 1.0 / static_cast<double>(N);
    double sum = 0.0;

    for (int i = i_begin; i < i_end; i++) {
        double x = h * i;

        int radius = N - i;

        for (int j = -radius; j <= radius; j++) {
            double y = h * j;

            int k_limit = static_cast<int>(
                floor(sqrt(static_cast<double>(radius * radius - j * j)))
            );

            for (int k = -k_limit; k <= k_limit; k++) {
                double z = h * k;
                sum += func(x, y, z);
            }
        }
    }

    return sum;
}

double run_default(int N, int rank, int size)
{
    int base = N / size;
    int rem = N % size;

    int i_begin = rank * base + std::min(rank, rem);
    int count = base + (rank < rem ? 1 : 0);
    int i_end = i_begin + count;

    double local_sum = compute_range(N, i_begin, i_end);
    double global_sum = 0.0;

    MPI_Reduce(
        &local_sum,
        &global_sum,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    return global_sum;
}

double run_static(int N, int rank, int size)
{
    double h = 1.0 / static_cast<double>(N);
    double local_sum = 0.0;

    for (int i = rank; i < N; i += size) {
        double x = h * i;

        int radius = N - i;

        for (int j = -radius; j <= radius; j++) {
            double y = h * j;

            int k_limit = static_cast<int>(
                floor(sqrt(static_cast<double>(radius * radius - j * j)))
            );

            for (int k = -k_limit; k <= k_limit; k++) {
                double z = h * k;
                local_sum += func(x, y, z);
            }
        }
    }

    double global_sum = 0.0;

    MPI_Reduce(
        &local_sum,
        &global_sum,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    return global_sum;
}

double run_dynamic(int N, int rank, int size, int chunk_size)
{
    if (size < 2) {
        if (rank == 0) {
            return compute_range(N, 0, N);
        }
        return 0.0;
    }

    double global_sum = 0.0;

    if (rank == 0) {
        int next_i = 0;
        int active_workers = 0;

        for (int worker = 1; worker < size; worker++) {
            if (next_i < N) {
                int task[2];

                task[0] = next_i;
                task[1] = next_i + chunk_size;

                if (task[1] > N) {
                    task[1] = N;
                }

                MPI_Send(task, 2, MPI_INT, worker, TAG_WORK, MPI_COMM_WORLD);

                next_i = task[1];
                active_workers++;
            } else {
                MPI_Send(nullptr, 0, MPI_INT, worker, TAG_STOP, MPI_COMM_WORLD);
            }
        }

        while (active_workers > 0) {
            double partial_sum = 0.0;
            MPI_Status status;

            MPI_Recv(
                &partial_sum,
                1,
                MPI_DOUBLE,
                MPI_ANY_SOURCE,
                TAG_RESULT,
                MPI_COMM_WORLD,
                &status
            );

            int worker = status.MPI_SOURCE;
            global_sum += partial_sum;

            if (next_i < N) {
                int task[2];

                task[0] = next_i;
                task[1] = next_i + chunk_size;

                if (task[1] > N) {
                    task[1] = N;
                }

                MPI_Send(task, 2, MPI_INT, worker, TAG_WORK, MPI_COMM_WORLD);

                next_i = task[1];
            } else {
                MPI_Send(nullptr, 0, MPI_INT, worker, TAG_STOP, MPI_COMM_WORLD);
                active_workers--;
            }
        }
    } else {
        while (true) {
            MPI_Status status;
            int task[2];

            MPI_Recv(
                task,
                2,
                MPI_INT,
                0,
                MPI_ANY_TAG,
                MPI_COMM_WORLD,
                &status
            );

            if (status.MPI_TAG == TAG_STOP) {
                break;
            }

            int i_begin = task[0];
            int i_end = task[1];

            double partial_sum = compute_range(N, i_begin, i_end);

            MPI_Send(
                &partial_sum,
                1,
                MPI_DOUBLE,
                0,
                TAG_RESULT,
                MPI_COMM_WORLD
            );
        }
    }

    return global_sum;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0) {
            std::cout << "Usage:\n";
            std::cout << "  mpirun -np <processes> ./task1 <N> <mode> [chunk_size]\n\n";
            std::cout << "Modes:\n";
            std::cout << "  default\n";
            std::cout << "  static\n";
            std::cout << "  dynamic\n\n";
            std::cout << "Examples:\n";
            std::cout << "  mpirun -np 4 ./task1 1000 default\n";
            std::cout << "  mpirun -np 4 ./task1 1000 static\n";
            std::cout << "  mpirun -np 4 ./task1 1000 dynamic 1\n";
        }

        MPI_Finalize();
        return 1;
    }

    int N = std::atoi(argv[1]);
    const char* mode = argv[2];

    int chunk_size = 1;

    if (argc >= 4) {
        chunk_size = std::atoi(argv[3]);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    double global_sum = 0.0;

    if (std::strcmp(mode, "default") == 0) {
        global_sum = run_default(N, rank, size);
    } else if (std::strcmp(mode, "static") == 0) {
        global_sum = run_static(N, rank, size);
    } else if (std::strcmp(mode, "dynamic") == 0) {
        global_sum = run_dynamic(N, rank, size, chunk_size);
    } else {
        if (rank == 0) {
            std::cout << "Unknown mode: " << mode << std::endl;
        }

        MPI_Finalize();
        return 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();

    double elapsed = t_end - t_start;

    if (rank == 0) {
        double h = 1.0 / static_cast<double>(N);
        double B = h * h * h * global_sum;

        std::cout << "N = " << N << std::endl;
        std::cout << "Processes = " << size << std::endl;
        std::cout << "Mode = " << mode << std::endl;

        if (std::strcmp(mode, "dynamic") == 0) {
            std::cout << "Chunk size = " << chunk_size << std::endl;
        }

        std::cout.precision(15);
        std::cout << "B = " << B << std::endl;
        std::cout << "Time = " << elapsed << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}