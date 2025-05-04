#include <thread>
#include <cstdio>
#include <windows.h>

unsigned int counter = 0;

void increment(int id){
    for(int i = 0;i<10;i++){
        counter++;
        Sleep(300);
    }

    //ten blok wykona się tylko raz mimo, że wątków jest więcej
    if(id == 1){
        printf("%u\n", counter);
    }
}

int main(){

    std::thread t1(increment,1);
    std::thread t2(increment,2);

    t1.join();
    t2.join();

    return 0;
}