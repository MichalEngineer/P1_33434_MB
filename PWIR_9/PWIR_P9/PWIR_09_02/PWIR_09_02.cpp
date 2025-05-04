#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include <omp.h>

#define VECTOR_SIZE 1000000

uint8_t* vector;

double vectorLengthParallel() {
    uint32_t sum = 0;

#pragma omp parallel for reduction(+:sum)
    for (int32_t i = 0; i < VECTOR_SIZE; i++) {
        sum += vector[i] * vector[i];
    }

    return sqrt(sum);
}

int main() {
    srand(time(NULL));

    vector = new uint8_t[VECTOR_SIZE];

    for (uint32_t i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = static_cast<uint8_t>(rand() % 11);
    }

    auto start = std::chrono::high_resolution_clock::now();
    double length = vectorLengthParallel();
    auto end = std::chrono::high_resolution_clock::now();

    printf("Dlugosc wektora (rownolegle): %.2f w czasie: %llu ms\n",
        length,
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    delete[] vector;
    return 0;
}