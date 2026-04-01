// Matr_Mult.cpp
#include <iostream>
#include <chrono>

using namespace std;

#define N 2048
double a[N][N], b[N][N], c[N][N];
using namespace std;
int main()
{
int i, j, k;
// инициализация матриц
for (i=0; i<N; i++)
for (j=0; j<N; j++)
a[i][j]=b[i][j]=i*j;
auto time_start = chrono::steady_clock::now();
// основной вычислительный блок
#pragma omp parallel for shared(a, b, c) private(i, j, k)
for(i=0; i<N; i++){
    for(j=0; j<N; j++){
        c[i][j] = 0.0;
        for(k=0; k<N; k++) c[i][j]+=a[i][k]*b[k][j];
    }
}
auto time_end = chrono::steady_clock::now();
auto msecs = chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
cout << "Time=" << msecs.count() << " millisec." << endl;
}