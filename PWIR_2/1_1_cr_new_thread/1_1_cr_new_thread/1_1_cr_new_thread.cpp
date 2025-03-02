#include <cstdio>
#include <thread>
#include <windows.h>

void action(int sleep_time) {
    printf("Uruchamiam watek %d\n", std::this_thread::get_id());
    Sleep(sleep_time * 1000); // Usypianie na podaną liczbę sekund
    printf("Koncze watek %d\n", std::this_thread::get_id());
}

int main() {

    std::thread t1(action, 5);  // Wątek usypia na 5 sekund
    std::thread t2(action, 10); // Wątek usypia na 10 sekund
    std::thread t3(action, 15); // Wątek usypia na 15 sekund

    t1.join(); 
    t2.join();
    t3.join();

    printf("Koniec programu \r\n");

    return 0;
}
