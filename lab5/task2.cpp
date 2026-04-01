#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <omp.h>

using namespace std;
using namespace std::chrono;

double f(double x, double y, double z) {
    return 1.0 / (2.0 + exp(-x * x) * sin(y * y + z * z));
}

// Последовательное вычисление суммы для конуса
double computeSequentialCone(int N) {
    double h = 1.0 / N;
    double sum = 0.0;

    for (int i = 0; i < N; ++i) {
        int r = N - i;

        for (int j = -r; j <= r; ++j) {
            int kMax = static_cast<int>(sqrt(1.0 * r * r - j * j));

            for (int k = -kMax; k <= kMax; ++k) {
                double x = h * i;
                double y = h * j;
                double z = h * k;

                sum += f(x, y, z);
            }
        }
    }

    return sum * h * h * h;
}

// 0 - default
// 1 - static
// 2 - dynamic
double computeParallelCone(int N, int scheduleType) {
    double h = 1.0 / N;
    double sum = 0.0;

    if (scheduleType == 0) {
        #pragma omp parallel for reduction(+:sum)
        for (int i = 0; i < N; ++i) {
            int r = N - i;

            for (int j = -r; j <= r; ++j) {
                int kMax = static_cast<int>(sqrt(1.0 * r * r - j * j));

                for (int k = -kMax; k <= kMax; ++k) {
                    double x = h * i;
                    double y = h * j;
                    double z = h * k;

                    sum += f(x, y, z);
                }
            }
        }
    } else if (scheduleType == 1) {
        #pragma omp parallel for reduction(+:sum) schedule(static)
        for (int i = 0; i < N; ++i) {
            int r = N - i;

            for (int j = -r; j <= r; ++j) {
                int kMax = static_cast<int>(sqrt(1.0 * r * r - j * j));

                for (int k = -kMax; k <= kMax; ++k) {
                    double x = h * i;
                    double y = h * j;
                    double z = h * k;

                    sum += f(x, y, z);
                }
            }
        }
    } else if (scheduleType == 2) {
        #pragma omp parallel for reduction(+:sum) schedule(dynamic)
        for (int i = 0; i < N; ++i) {
            int r = N - i;

            for (int j = -r; j <= r; ++j) {
                int kMax = static_cast<int>(sqrt(1.0 * r * r - j * j));

                for (int k = -kMax; k <= kMax; ++k) {
                    double x = h * i;
                    double y = h * j;
                    double z = h * k;

                    sum += f(x, y, z);
                }
            }
        }
    }

    return sum * h * h * h;
}

double measureSequential(int N, double &result) {
    auto start = high_resolution_clock::now();
    result = computeSequentialCone(N);
    auto end = high_resolution_clock::now();
    return duration<double>(end - start).count();
}

double measureParallel(int N, int scheduleType, double &result) {
    auto start = high_resolution_clock::now();
    result = computeParallelCone(N, scheduleType);
    auto end = high_resolution_clock::now();
    return duration<double>(end - start).count();
}

string scheduleName(int type) {
    if (type == 0) return "default";
    if (type == 1) return "static";
    return "dynamic";
}

int main() {
    setlocale(LC_ALL, "");
    cout << fixed << setprecision(10);

    vector<int> Ns = {1000, 1500, 2000};
    int threads = omp_get_max_threads();

    cout << "Число потоков OpenMP: " << threads << "\n\n";

    for (int N : Ns) {
        double seqResult = 0.0;
        double tSeq = measureSequential(N, seqResult);

        cout << "==============================\n";
        cout << "Конус, N = " << N << "\n";
        cout << "Последовательный алгоритм:\n";
        cout << "B = " << seqResult << "\n";
        cout << "T_seq = " << tSeq << " сек\n\n";

        for (int sched = 0; sched < 3; ++sched) {
            double parResult = 0.0;
            double tPar = measureParallel(N, sched, parResult);

            double speedup = tSeq / tPar;
            double efficiency = speedup / threads;

            cout << "Параллельный алгоритм, schedule(" << scheduleName(sched) << "):\n";
            cout << "B = " << parResult << "\n";
            cout << "T_par = " << tPar << " сек\n";
            cout << "Ускорение S = " << speedup << "\n";
            cout << "Эффективность E = " << efficiency << "\n\n";
        }
    }

    return 0;
}