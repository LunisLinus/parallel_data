#include <iostream>
#include <vector>
#include <numeric>
#include <omp.h>

// Три "разные" работы (конечный набор, не цикл)
double heavy_sum(const std::vector<double>& a) {
    return std::accumulate(a.begin(), a.end(), 0.0);
}

double heavy_dot(const std::vector<double>& a, const std::vector<double>& b) {
    double s = 0.0;
    for (size_t i = 0; i < a.size(); ++i) s += a[i] * b[i];
    return s;
}

double heavy_max(const std::vector<double>& a) {
    double m = a[0];
    for (double x : a) if (x > m) m = x;
    return m;
}

int main() {
    const size_t n = 5'000'000;
    std::vector<double> a(n, 1.000001), b(n, 0.999999);

    double sum = 0.0, dot = 0.0, mx = 0.0;

    double t0 = omp_get_wtime();

    // parallel sections = parallel + sections (комбинированная форма)
#pragma omp parallel sections
    {
#pragma omp section
        {
            sum = heavy_sum(a);
            std::cout << "sum  done by thread " << omp_get_thread_num() << "\n";
        }

#pragma omp section
        {
            dot = heavy_dot(a, b);
            std::cout << "dot  done by thread " << omp_get_thread_num() << "\n";
        }

#pragma omp section
        {
            mx = heavy_max(a);
            std::cout << "max  done by thread " << omp_get_thread_num() << "\n";
        }
    } // по умолчанию барьер в конце sections: все результаты уже готовы

    double t1 = omp_get_wtime();

    std::cout << "sum=" << sum << "\n";
    std::cout << "dot=" << dot << "\n";
    std::cout << "max=" << mx << "\n";
    std::cout << "elapsed(s)=" << (t1 - t0) << "\n";
}