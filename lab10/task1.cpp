#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TAG_WORK 1
#define TAG_STOP 2
#define TAG_RESULT 3


double func(double x, double y, double z)
{
    return 1.0 / (2.0 + exp(-x * x) * sin(y * y + z * z));
}

/*
    Считает сумму по диапазону i: [i_begin, i_end)
*/
double compute_range(int N, int i_begin, int i_end)
{
    double h = 1.0 / (double)N;
    double sum = 0.0;

    for (int i = i_begin; i < i_end; i++) {
        double x = h * ((double)i + 0.5);

        for (int j = 0; j < N; j++) {
            double y = h * ((double)j + 0.5);

            for (int k = 0; k < N; k++) {
                double z = h * ((double)k + 0.5);
                sum += func(x, y, z);
            }
        }
    }

    return sum;
}

/*
    Вариант default:
    каждый процесс получает один непрерывный блок индексов i.
*/
double run_default(int N, int rank, int size)
{
    int base = N / size;
    int rem = N % size;

    int i_begin = rank * base + (rank < rem ? rank : rem);
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

/*
    Вариант static:
    циклическое распределение:
    процесс rank считает i = rank, rank + size, rank + 2 * size, ...
*/
double run_static(int N, int rank, int size)
{
    double h = 1.0 / (double)N;
    double local_sum = 0.0;

    for (int i = rank; i < N; i += size) {
        double x = h * ((double)i + 0.5);

        for (int j = 0; j < N; j++) {
            double y = h * ((double)j + 0.5);

            for (int k = 0; k < N; k++) {
                double z = h * ((double)k + 0.5);
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

/*
    Вариант dynamic:
    процесс 0 — master.
    Остальные процессы — workers.

    Master выдаёт блоки индексов i.
    Worker получает блок, считает его и отправляет результат.
*/
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

        /*
            Сначала отправляем стартовые задачи всем worker-процессам.
        */
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
                MPI_Send(NULL, 0, MPI_INT, worker, TAG_STOP, MPI_COMM_WORLD);
            }
        }

        /*
            Принимаем результаты и выдаём новые задачи.
        */
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
                MPI_Send(NULL, 0, MPI_INT, worker, TAG_STOP, MPI_COMM_WORLD);
                active_workers--;
            }
        }
    } else {
        while (1) {
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
            printf("Usage:\n");
            printf("  mpirun -np <processes> ./main <N> <mode> [chunk_size]\n\n");
            printf("Modes:\n");
            printf("  default\n");
            printf("  static\n");
            printf("  dynamic\n\n");
            printf("Examples:\n");
            printf("  mpirun -np 4 ./main 1000 default\n");
            printf("  mpirun -np 4 ./main 1000 static\n");
            printf("  mpirun -np 4 ./main 1000 dynamic 4\n");
        }

        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);
    char mode[32];
    strcpy(mode, argv[2]);

    int chunk_size = 4;

    if (argc >= 4) {
        chunk_size = atoi(argv[3]);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    double global_sum = 0.0;

    if (strcmp(mode, "default") == 0) {
        global_sum = run_default(N, rank, size);
    } else if (strcmp(mode, "static") == 0) {
        global_sum = run_static(N, rank, size);
    } else if (strcmp(mode, "dynamic") == 0) {
        global_sum = run_dynamic(N, rank, size, chunk_size);
    } else {
        if (rank == 0) {
            printf("Unknown mode: %s\n", mode);
        }

        MPI_Finalize();
        return 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();

    double elapsed = t_end - t_start;

    if (rank == 0) {
        double h = 1.0 / (double)N;
        double A = h * h * h * global_sum;

        printf("N = %d\n", N);
        printf("Processes = %d\n", size);
        printf("Mode = %s\n", mode);

        if (strcmp(mode, "dynamic") == 0) {
            printf("Chunk size = %d\n", chunk_size);
        }

        printf("Integral average A = %.15f\n", A);
        printf("Time = %.6f seconds\n", elapsed);
    }

    MPI_Finalize();
    return 0;
}