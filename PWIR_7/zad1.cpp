#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Struktura do przekazywania argumentów do wątku
typedef struct {
    long long num_points;
    int thread_id;
    long long points_inside_circle;
} ThreadData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Funkcja wykonywana przez wątek
void* calculate_points(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    unsigned int seed = data->thread_id;
    long long local_points_inside = 0;
    
    for (long long i = 0; i < data->num_points; i++) {
        // Generowanie losowych współrzędnych w przedziale [0,1]
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        
        // Sprawdzenie czy punkt leży w ćwiartce koła
        if (x*x + y*y <= 1.0) {
            local_points_inside++;
        }
    }
    
    data->points_inside_circle = local_points_inside;
    return NULL;
}

int main() {
    long long num_points = 100000000;
    int num_threads = 8;
    
    // Alokacja pamięci dla struktur danych wątków
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    
    long long points_per_thread = num_points / num_threads;
    
    clock_t start_time = clock();
    
    // Tworzenie wątków
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].num_points = points_per_thread;
        thread_data[i].thread_id = i;
        thread_data[i].points_inside_circle = 0;
        
        if (pthread_create(&threads[i], NULL, calculate_points, &thread_data[i]) != 0) {
            perror("Błąd tworzenia wątku");
            exit(EXIT_FAILURE);
        }
    }
    
    // Oczekiwanie na zakończenie wątków
    long long total_points_inside = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_points_inside += thread_data[i].points_inside_circle;
    }
    
    // Obliczenie przybliżonej wartości PI
    double pi_approx = 4.0 * total_points_inside / (points_per_thread * num_threads);
    
    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("Przybliżona wartość PI: %f\n", pi_approx);
    printf("Czas wykonania: %f sekund\n", execution_time);
    printf("Liczba wątków: %d\n", num_threads);
    
    // Zwolnienie pamięci
    free(threads);
    free(thread_data);
    
    return 0;
}