#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <cassert>
#include <ctime>
#include <omp.h>

#define MATRIX_H 30000
#define MATRIX_W 30000
#define VECTOR_S 30000
#define ITERATIONS 5

uint16_t** matrix;
uint16_t* vector;
uint16_t* result;

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Poprawka C4244: konwersja z time_t do unsigned int
    assert(MATRIX_W == VECTOR_S);

    matrix = new uint16_t * [MATRIX_H];
    for (int i = 0; i < MATRIX_H; i++)
        matrix[i] = new uint16_t[MATRIX_W];

    vector = new uint16_t[VECTOR_S];
    result = new uint16_t[MATRIX_H]; 

    auto seq_total_start = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < ITERATIONS; iter++) {

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < MATRIX_H; i++) {
            for (int k = 0; k < MATRIX_W; k++) {
                matrix[i][k] = static_cast<uint16_t>(rand() % 100);
            }
        }

        for (int i = 0; i < VECTOR_S; i++) {
            vector[i] = static_cast<uint16_t>(rand() % 100);
        }
        for (int i = 0; i < MATRIX_H; i++) {
            result[i] = 0;
        }
        auto end = std::chrono::high_resolution_clock::now();
        printf("Iteracja %d: Wypelnienie sekwencyjne w %lld ms\n",
            iter + 1, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < MATRIX_H; i++) {
            for (int k = 0; k < MATRIX_W; k++) {
                result[i] += matrix[i][k] * vector[k];
            }
        }
        end = std::chrono::high_resolution_clock::now();
        printf("Iteracja %d: Mnozenie sekwencyjne w %lld ms\n",
            iter + 1, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    }

    auto seq_total_end = std::chrono::high_resolution_clock::now();
    printf("\nCalkowity czas sekwencyjny dla %d iteracji: %lld ms\n\n", ITERATIONS,
        std::chrono::duration_cast<std::chrono::milliseconds>(seq_total_end - seq_total_start).count());

    auto par_total_start = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < ITERATIONS; iter++) {
        auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for num_threads(4) shared(matrix)
        for (int i = 0; i < MATRIX_H; i++) {
            for (int k = 0; k < MATRIX_W; k++) {
                matrix[i][k] = static_cast<uint16_t>(rand() % 100);
            }
        }

#pragma omp parallel for num_threads(4) shared(vector, result)
        for (int i = 0; i < VECTOR_S; i++) {
            vector[i] = static_cast<uint16_t>(rand() % 100);
        }
#pragma omp parallel for num_threads(4) shared(result)
        for (int i = 0; i < MATRIX_H; i++) {
            result[i] = 0;
        }
        auto end = std::chrono::high_resolution_clock::now();
        printf("Iteracja %d: Wypelnienie rownolegle w %lld ms\n",
            iter + 1, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

        start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for num_threads(4) shared(matrix, vector, result)
        for (int i = 0; i < MATRIX_H; i++) {
            for (int k = 0; k < MATRIX_W; k++) {
                result[i] += matrix[i][k] * vector[k];
            }
        }
        end = std::chrono::high_resolution_clock::now();
        printf("Iteracja %d: Mnozenie rownolegle w %lld ms\n",
            iter + 1, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    }

    auto par_total_end = std::chrono::high_resolution_clock::now();
    printf("\nCalkowity czas rownolegly dla %d iteracji: %lld ms\n", ITERATIONS,
        std::chrono::duration_cast<std::chrono::milliseconds>(par_total_end - par_total_start).count());

    double seq_time = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(seq_total_end - seq_total_start).count());
    double par_time = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(par_total_end - par_total_start).count());
    printf("\nPrzyspieszenie: %.2fx\n", seq_time / par_time);

    delete[] vector;
    delete[] result;

    for (int i = 0; i < MATRIX_H; i++)
        delete[] matrix[i];

    delete[] matrix;

    return 0;
}
