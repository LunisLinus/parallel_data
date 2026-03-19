#include <iostream>
#include <omp.h>

#ifdef _WIN32
    #include <windows.h>
    void sleep_sec(int sec) {
        Sleep(sec * 1000);
    }
#else
#include <unistd.h>
void sleep_sec(int sec) {
        sleep(sec);
    }
#endif

int main() {
    omp_lock_t lock;
    omp_init_lock(&lock);

#pragma omp parallel num_threads(4)
    {
        int tid = omp_get_thread_num();

        // Пытаемся захватить блокировку
        while (!omp_test_lock(&lock)) {
#pragma omp critical
            {
                std::cout << "Поток " << tid
                          << " ожидает: секция заблокирована" << std::endl;
            }
            sleep_sec(1);
        }

        // Критическая секция
        std::cout << "Поток " << tid << " вошел в секцию" << std::endl;
        sleep_sec(2);
        std::cout << "Поток " << tid << " выходит из секции" << std::endl;

        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);
    return 0;
}