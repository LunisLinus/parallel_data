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

// Последовательное вычисление
double computeSequential(int N) {
    double h = 1.0 / N;
    double sum = 0.0;

    for (int i = 0; i < N; ++i) {
        double x = h * (i + 0.5);
        for (int j = 0; j < N; ++j) {
            double y = h * (j + 0.5);
            for (int k = 0; k < N; ++k) {
                double z = h * (k + 0.5);
                sum += f(x, y, z);
            }
        }
    }

    return sum * h * h * h;
}

// 0 - default
// 1 - static
// 2 - dynamic
double computeParallel(int N, int scheduleType) {
    double h = 1.0 / N;
    double sum = 0.0;

    if (scheduleType == 0) {
        #pragma omp parallel for reduction(+:sum) collapse(3)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int k = 0; k < N; ++k) {
                    double x = h * (i + 0.5);
                    double y = h * (j + 0.5);
                    double z = h * (k + 0.5);
                    sum += f(x, y, z);
                }
            }
        }
    } else if (scheduleType == 1) {
        #pragma omp parallel for reduction(+:sum) collapse(3) schedule(static)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int k = 0; k < N; ++k) {
                    double x = h * (i + 0.5);
                    double y = h * (j + 0.5);
                    double z = h * (k + 0.5);
                    sum += f(x, y, z);
                }
            }
        }
    } else if (scheduleType == 2) {
        #pragma omp parallel for reduction(+:sum) collapse(3) schedule(dynamic)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int k = 0; k < N; ++k) {
                    double x = h * (i + 0.5);
                    double y = h * (j + 0.5);
                    double z = h * (k + 0.5);
                    sum += f(x, y, z);
                }
            }
        }
    }

    return sum * h * h * h;
}

double measureSequential(int N, double &result) {
    auto start = high_resolution_clock::now();
    result = computeSequential(N);
    auto end = high_resolution_clock::now();
    return duration<double>(end - start).count();
}

double measureParallel(int N, int scheduleType, double &result) {
    auto start = high_resolution_clock::now();
    result = computeParallel(N, scheduleType);
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
        cout << "N = " << N << "\n";
        cout << "Последовательный алгоритм:\n";
        cout << "A = " << seqResult << "\n";
        cout << "T_seq = " << tSeq << " сек\n\n";

        for (int sched = 0; sched < 3; ++sched) {
            double parResult = 0.0;
            double tPar = measureParallel(N, sched, parResult);

            double speedup = tSeq / tPar;
            double efficiency = speedup / threads;

            cout << "Параллельный алгоритм, schedule(" << scheduleName(sched) << "):\n";
            cout << "A = " << parResult << "\n";
            cout << "T_par = " << tPar << " сек\n";
            cout << "Ускорение S = " << speedup << "\n";
            cout << "Эффективность E = " << efficiency << "\n\n";
        }
    }

    return 0;
}