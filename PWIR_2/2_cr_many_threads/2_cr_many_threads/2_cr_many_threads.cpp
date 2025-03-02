#include <cstdio>
#include <thread>
#include <windows.h>
#include <vector>

void action(int id) {
    printf("Uruchamiam watek %d\n", id);
    Sleep(5 * 1000); // 5 sekund
    printf("Koncze watek %d\n", id);
}

int main() {
    int thread_count;

    printf("Podaj liczbe watkow: ");
    scanf_s("%d", &thread_count);

    std::vector<std::thread*> threads;

    for (int i = 0; i < thread_count; i++) {
        threads.push_back(new std::thread(action, i));
    }

    for (int i = 0; i < thread_count; i++) {
        threads[i]->join();
    }

    for (int i = 0; i < thread_count; i++) {
        delete threads[i];
    }

    printf("Koniec programu \r\n");

    return 0;
}
