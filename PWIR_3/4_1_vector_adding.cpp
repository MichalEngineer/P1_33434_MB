#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <thread>
#include <chrono>

#define SIZE 100
#define CHUNK 10
#define THREAD_COUNT (SIZE / CHUNK)

void add(int id, int* a, int* b, int* c) {
    int start = id * CHUNK;
    int end = start + CHUNK;
    for (int i = start; i < end; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    srand(time(NULL));
    int a[SIZE];
    int b[SIZE];
    int c[SIZE];

    for (int i = 0; i < SIZE; i++) {
        a[i] = rand() % 100 + 1; 
        b[i] = rand() % 100 + 1;
    }

    for (int i = 0; i < SIZE; i++) {
        printf("%u ", a[i]);
    }
    printf("\n");

    for (int i = 0; i < SIZE; i++) {
        printf("%u ", b[i]);
    }
    printf("\n");

    std::thread* threads[THREAD_COUNT];

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < THREAD_COUNT; i++) {
        threads[i] = new std::thread(add, i, a, b, c);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        threads[i]->join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    for (int i = 0; i < THREAD_COUNT; i++) {
        delete threads[i];
    }

    for (int i = 0; i < SIZE; i++) {
        printf("%u ", c[i]);
    }
    printf("\n");

    printf("Czas wykonania operacji dodawania: %f sekundy\n", elapsed.count());

    return 0;
}