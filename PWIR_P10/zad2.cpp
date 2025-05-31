#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <chrono>
#include <omp.h>

#define VECTOR_SIZE 100000000

double* vector;
double length = 0.0;

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    printf("Inicjalizacja wektora o rozmiarze %d...\n", VECTOR_SIZE);
    vector = new double[VECTOR_SIZE];

    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = (double)(rand() % 100) / 10.0;
    }

    printf("\nObliczanie dlugosci wektora - metoda sekwencyjna...\n");
    auto start = std::chrono::high_resolution_clock::now();

    double seqLength = 0.0;
    for (int i = 0; i < VECTOR_SIZE; i++) {
        seqLength += vector[i] * vector[i];
    }
    seqLength = sqrt(seqLength);

    auto end = std::chrono::high_resolution_clock::now();
    printf("Czas sekwencyjny: %lld ms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    printf("Dlugosc wektora (sekwencyjnie): %f\n", seqLength);

    printf("\nObliczanie dlugosci wektora - metoda rownolegle z sekcjami...\n");
    start = std::chrono::high_resolution_clock::now();

    double parallelLength = 0.0;
    double partialSum[4] = { 0.0, 0.0, 0.0, 0.0 };

#pragma omp parallel num_threads(4)
    {
#pragma omp sections
        {
#pragma omp section
            {
                int tid = omp_get_thread_num();
                printf("Watek %d rozpoczyna prace na sekcji 1\n", tid);
                for (int i = 0; i < VECTOR_SIZE / 4; i++) {
                    partialSum[0] += vector[i] * vector[i];
                }
            }

#pragma omp section
            {
                int tid = omp_get_thread_num();
                printf("Watek %d rozpoczyna prace na sekcji 2\n", tid);
                for (int i = VECTOR_SIZE / 4; i < VECTOR_SIZE / 2; i++) {
                    partialSum[1] += vector[i] * vector[i];
                }
            }

#pragma omp section
            {
                int tid = omp_get_thread_num();
                printf("Watek %d rozpoczyna prace na sekcji 3\n", tid);
                for (int i = VECTOR_SIZE / 2; i < 3 * VECTOR_SIZE / 4; i++) {
                    partialSum[2] += vector[i] * vector[i];
                }
            }

#pragma omp section
            {
                int tid = omp_get_thread_num();
                printf("Watek %d rozpoczyna prace na sekcji 4\n", tid);
                for (int i = 3 * VECTOR_SIZE / 4; i < VECTOR_SIZE; i++) {
                    partialSum[3] += vector[i] * vector[i];
                }
            }
        } 
    }

    parallelLength = sqrt(partialSum[0] + partialSum[1] + partialSum[2] + partialSum[3]);

    end = std::chrono::high_resolution_clock::now();
    printf("Czas rownolegly: %lld ms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    printf("Dlugosc wektora (rownolegle): %f\n", parallelLength);

    printf("\nRoznica miedzy wynikami: %e\n", fabs(seqLength - parallelLength));

    delete[] vector;

    return 0;
}