// Lin_Solv_LAPACK.cpp
#include <chrono>
#include <iostream>


#ifdef _WIN32
#define DGESV_Func DGESV
#else
#define DGESV_Func dgesv_
#endif

extern "C" void DGESV_Func(int* n, int* nrhs, double* a, int* lda,
	                       int* ipiv, double* b, int* ldb, int* info );

using namespace std;

#define NN 2048
double a[NN][NN], b[NN];
int IPIV[NN];
int N = NN;

int main(void)
{
 for (int i=1; i<=N; i++){
	for (int j=1; j<=N; j++)
	{double DI=i-j;
	a[j - 1][i - 1] =1.0/(1.0+DI*DI)+1.0/(2.0+DI*DI*DI); // Инициализация матрицы системы
     }
	b[i-1] =1+1.0/(i*i);
 }
int nrhs=1;
int info;
auto time_start = chrono::steady_clock::now();
DGESV_Func(&N, &nrhs, &a[0][0], &N, IPIV, &b[0], &N, &info);
auto time_end = chrono::steady_clock::now();
auto msecs = chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
cout << "Time=" << msecs.count() << " millisec." << endl;
cout<<"info="<<info<<endl;
}

