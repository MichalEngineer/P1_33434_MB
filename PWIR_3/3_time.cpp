#include <chrono>
#include <cstdio>

void fibonacci(int n) {
    long long a = 0, b = 1, temp;
    for (int i = 0; i < n; i++) {
        printf("%lld ", a);
        temp = a + b;
        a = b;
        b = temp;
    }
    printf("\n");
}

int main() {
    auto start_fib = std::chrono::steady_clock::now();
    fibonacci(40);
    auto end_fib = std::chrono::steady_clock::now();

    printf("Czas generowania Fibonacciego: %llu µs\n",
        std::chrono::duration_cast<std::chrono::microseconds>(end_fib - start_fib).count());

    return 0;
}
