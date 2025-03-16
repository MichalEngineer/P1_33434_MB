#include <chrono>
#include <cstdio>
#include <windows.h>

int main(){
    auto start = std::chrono::steady_clock::now();
    //długie operacje
    Sleep(2000);
    auto end = std::chrono::steady_clock::now();

    printf("Czas trwania: %llu\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return 0;
}