#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <assert.h>
#include <omp.h>
#include <time.h>

#define MATRIX_H 30000
#define MATRIX_W 30000

//operators
//+
//-
//*
//&
//|
//^
//&&
//||

uint8_t** matrix;

uint32_t sumMatrix(int p, int threads) {
    uint32_t sum = 0;
    int32_t i;
    int32_t k;

#pragma omp parallel for \
        if (p) num_threads(threads) \
        shared(matrix) private(k, i) \
        reduction(+ : sum)
    for (uint32_t i = 0; i < MATRIX_H; i++) {
        for (uint32_t k = 0; k < MATRIX_W; k++) {
            sum = sum + matrix[i][k];
        }
    }

    return sum;
}

int main() {
    srand(time(NULL));

    //alloc matrix
    matrix = (uint8_t**)new uint8_t * [MATRIX_H];
    for (uint32_t i = 0; i < MATRIX_H; i++)
        matrix[i] = new uint8_t[MATRIX_W];

    //fill matrix random data normal way
    for (uint32_t i = 0; i < MATRIX_H; i++) {
        for (uint32_t k = 0; k < MATRIX_W; k++) {
            matrix[i][k] = (uint16_t)(rand() % 10);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    uint32_t sum = sumMatrix(0, 0);
    auto end = std::chrono::high_resolution_clock::now();

    printf("Sum calculated normal way: %u in time: %llu ms\r\n", sum,
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    start = std::chrono::high_resolution_clock::now();
    sum = sumMatrix(1, 8);
    end = std::chrono::high_resolution_clock::now();

    printf("Sum calculated parralel way: %u in time: %llu ms\r\n", sum,
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    for (uint32_t i = 0; i < MATRIX_H; i++) delete[] matrix[i];
    delete[] matrix;

    return 0;
}