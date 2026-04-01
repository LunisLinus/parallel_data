#include <iostream>
#include <omp.h>

using namespace std;

// Глобальная переменная
int x = 10;

// Делаем её поток-локальной
#pragma omp threadprivate(x)

int main() {
    // Главный поток меняет значение своей копии
    x = 42;

    cout << "До parallel: x в главном потоке = " << x << endl;

    // copyin копирует значение x из главного потока
    // во все threadprivate-копии при входе в parallel
#pragma omp parallel copyin(x)
    {
        int tid = omp_get_thread_num();

#pragma omp critical
        {
            cout << "Поток " << tid
                 << ": начальное значение x = " << x << endl;
        }

        // Для наглядности каждый поток меняет свою копию
        x += tid;

#pragma omp critical
        {
            cout << "Поток " << tid
                 << ": изменённое значение x = " << x << endl;
        }
    }

    // После parallel у главного потока останется своя копия
    cout << "После parallel: x в главном потоке = " << x << endl;

    return 0;
}