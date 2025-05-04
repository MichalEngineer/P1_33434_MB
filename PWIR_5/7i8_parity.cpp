#include <thread>
#include <cstdio>
#include <windows.h>
#include <chrono>

unsigned int counter = 0;

void increment() {
    int iterations = 0;
    auto start = std::chrono::high_resolution_clock::now();

    for (;;) {
        counter++;
        iterations++;

        if (iterations >= 20) {
            break;
        }

        Sleep(2000);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("Increment time: %lld ms\n", duration.count());
}

void parity() {
    int iterations = 0;
    auto start = std::chrono::high_resolution_clock::now();

    for (;;) {
        if (counter % 2) {
            printf("%u jest nieparzyste\r\n", counter);
        }
        else {
            printf("%u jest parzyste\r\n", counter);
        }

        iterations++;

        if (iterations >= 20) {
            break;
        }

        Sleep(2000);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("Parity time: %lld ms\n", duration.count());
}

int main() {
    std::thread inc(increment);
    std::thread par(parity);

    inc.join();
    par.join();

    printf("Done\r\n");

    return 0;
}
