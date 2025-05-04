#include <cstdio>
#include <thread>
#include <atomic>

//unsigned long long sum2 = 0;
//std::atomic_uint64_t sum2 = 0;
std::atomic<unsigned long long> sum2(0);

void sum(unsigned char* data, int id, int count){
    for(unsigned i = id*count;i<(id+1)*count;i++){
        sum2+=data[i];
    }
}

int main(){
    unsigned char* data = new unsigned char[10000];

    for(unsigned int i = 0;i<10000;i++){
        data[i] = i;
    }

    //suma na jednym wątku
    unsigned long long sum1 = 0;
    for(unsigned i = 0;i<10000;i++){
        sum1+=data[i];
    }
    printf("Suma na 1 watku: %llu\r\n", sum1);

    //suma na wielu wątkach
    std::thread t1(sum, data, 0, 2000);
    std::thread t2(sum, data, 1, 2000);
    std::thread t3(sum, data, 2, 2000);
    std::thread t4(sum, data, 3, 2000);
    std::thread t5(sum, data, 4, 2000);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    //printf("Suma na 5 watkach: %llu\r\n", sum2);
    printf("Suma na 5 watkach: %llu\r\n", sum2.load());
}