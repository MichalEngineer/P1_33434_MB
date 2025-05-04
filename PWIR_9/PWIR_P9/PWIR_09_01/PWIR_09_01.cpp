#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <assert.h>
#include <time.h>
#include <omp.h>

#define MATRIX_H 3000
#define MATRIX_W 3000
#define VECTOR_S 3000
#define MUL_TIME 25
#define TEST_COUNT 5

uint16_t** matrix;
uint16_t* vector;
uint16_t* result;

int main() {
    srand(time(NULL));
    assert(MATRIX_W == VECTOR_S);

    matrix = new uint16_t * [MATRIX_H];
    for (int i = 0; i < MATRIX_H; i++)
        matrix[i] = new uint16_t[MATRIX_W];

    vector = new uint16_t[VECTOR_S];
    result = new uint16_t[MATRIX_H];

#pragma omp parallel for
    for (int i = 0; i < MATRIX_H; i++)
        for (int k = 0; k < MATRIX_W; k++)
            matrix[i][k] = rand() % 100;

#pragma omp parallel for
    for (int i = 0; i < VECTOR_S; i++)
        vector[i] = rand() % 100;

    // RESET FUNCTION
    auto reset_result = []() {
#pragma omp parallel for
        for (int i = 0; i < MATRIX_H; i++)
            result[i] = 0;
        };

    // SCHEDULE TEST FUNCTION
    auto test_schedule = [&](const char* label, auto schedule_lambda) {
        printf("\n--- %s ---\n", label);
        uint64_t total_time = 0;

        for (int test = 0; test < TEST_COUNT; test++) {
            reset_result();
            auto start = std::chrono::high_resolution_clock::now();
            for (int p = 0; p < MUL_TIME; p++) {
                schedule_lambda();
            }
            auto end = std::chrono::high_resolution_clock::now();
            uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            total_time += ms;
            printf("Run %d: %llu ms\n", test + 1, ms);
        }

        printf("Average: %llu ms\n", total_time / TEST_COUNT);
        };

    // Static
    test_schedule("Static", []() {
#pragma omp parallel for schedule(static) shared(matrix, vector, result)
        for (int i = 0; i < MATRIX_H; i++)
            for (int k = 0; k < MATRIX_W; k++)
                result[i] += matrix[i][k] * vector[k];
        });

    // Dynamic
    test_schedule("Dynamic, chunk = MATRIX_H/10", []() {
#pragma omp parallel for schedule(dynamic, MATRIX_H/10) shared(matrix, vector, result)
        for (int i = 0; i < MATRIX_H; i++)
            for (int k = 0; k < MATRIX_W; k++)
                result[i] += matrix[i][k] * vector[k];
        });

    // Guided
    test_schedule("Guided, chunk = MATRIX_H/10", []() {
#pragma omp parallel for schedule(guided, MATRIX_H/10) shared(matrix, vector, result)
        for (int i = 0; i < MATRIX_H; i++)
            for (int k = 0; k < MATRIX_W; k++)
                result[i] += matrix[i][k] * vector[k];
        });

    // Runtime
    test_schedule("Runtime (from OMP_SCHEDULE)", []() {
#pragma omp parallel for schedule(runtime) shared(matrix, vector, result)
        for (int i = 0; i < MATRIX_H; i++)
            for (int k = 0; k < MATRIX_W; k++)
                result[i] += matrix[i][k] * vector[k];
        });

    delete[] vector;
    delete[] result;
    for (int i = 0; i < MATRIX_H; i++)
        delete[] matrix[i];
    delete[] matrix;

    return 0;
}
