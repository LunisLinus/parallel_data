// Matr_Mult_BLAS.cpp
#include <chrono>
#include <iostream>


#ifdef _WIN32
#define DGEMM_Func DGEMM
#else
#define DGEMM_Func dgemm_
#endif

extern "C" void DGEMM_Func(char *transa, char *transb, 
	                  int *m, int *n, int *k,
                      double *alpha, double *a, int *lda, 
					  double *b, const int *ldb,
                      double *beta, double *c, int *ldc);
using namespace std;
#define NN 2048
double a[NN][NN], b[NN][NN], c[NN][NN];
int N = NN;

int main(void)
{
	
 
for (int i=0; i<N; i++)
	for (int j=0; j<N; j++)
	{a[j][i] =i*j; // Инициализация матриц
     b[j][i] =i*j;
	}
double alpha=1, beta=0;
char transa='N', transb='N';
auto time_start = chrono::steady_clock::now();
DGEMM_Func(&transa, &transb, &N, &N, &N,
      &alpha, &a[0][0], &N, &b[0][0], &N,
      &beta, &c[0][0], &N); //Перемножение матриц
auto time_end = chrono::steady_clock::now();
auto msecs = chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
cout.precision(8);
cout << "Time=" << msecs.count() << " millisec." << endl;


}


