#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Struktura do przechowywania parametrów dla wątku
typedef struct {
    double** A;
    double** B;
    double** C;
    int n, m, p;
    int start_row;
    int end_row;
} ThreadData;

// Funkcja do alokacji macierzy
double** allocate_matrix(int rows, int cols) {
    double** matrix = (double**)malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(cols * sizeof(double));
    }
    return matrix;
}

// Funkcja do inicjalizacji macierzy losowymi wartościami
void initialize_random_matrix(double** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

// Funkcja do zwalniania pamięci macierzy
void free_matrix(double** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Funkcja wykonywana przez wątek - mnoży fragment macierzy
void* multiply_matrix_block(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->p; j++) {
            data->C[i][j] = 0.0;
            for (int k = 0; k < data->m; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }
    
    return NULL;
}

int main() {
    int n = 1000;  // Liczba wierszy A
    int m = 1000;  // Liczba kolumn A / wierszy B
    int p = 1000;  // Liczba kolumn B
    int num_threads = 8;
    
    // Inicjalizacja generatora liczb losowych
    srand(time(NULL));
    
    // Alokacja i inicjalizacja macierzy
    double** A = allocate_matrix(n, m);
    double** B = allocate_matrix(m, p);
    double** C = allocate_matrix(n, p);
    
    initialize_random_matrix(A, n, m);
    initialize_random_matrix(B, m, p);
    
    // Alokacja pamięci dla wątków i ich danych
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    
    // Obliczenie liczby wierszy na wątek
    int rows_per_thread = n / num_threads;
    
    // Pomiar czasu
    clock_t start_time = clock();
    
    // Tworzenie wątków
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C;
        thread_data[i].n = n;
        thread_data[i].m = m;
        thread_data[i].p = p;
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == num_threads - 1) ? n : (i + 1) * rows_per_thread;
        
        if (pthread_create(&threads[i], NULL, multiply_matrix_block, &thread_data[i]) != 0) {
            perror("Błąd tworzenia wątku");
            exit(EXIT_FAILURE);
        }
    }
    
    // Oczekiwanie na zakończenie wątków
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("Czas równoległego mnożenia macierzy: %f sekund\n", execution_time);
    printf("Liczba wątków: %d\n", num_threads);
    
    // Zwolnienie pamięci
    free_matrix(A, n);
    free_matrix(B, m);
    free_matrix(C, n);
    free(threads);
    free(thread_data);
    
    return 0;
}