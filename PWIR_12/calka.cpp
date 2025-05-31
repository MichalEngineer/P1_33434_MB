#include "mpi.h"
#include <iostream>
#include <cmath>
#include <iomanip> // do precyzyjnego formatowania

const double PI = 3.14159265358979323846;

// Funkcja do całkowania: f(x) = sin(x)
double f(double x) {
    return sin(x);
}

int main(int argc, char** argv) {
    int rank, size;
    double a = 0.0;         // dolna granica całkowania
    double b = PI;          // górna granica całkowania 
    int n = 1000000;        // liczba prostokątów
    double h = (b - a) / n; // szerokość prostokąta
    double local_sum = 0.0; // suma częściowa dla procesu
    double total_sum = 0.0; // suma całkowita

    // Inicjalizacja MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Obliczanie liczby prostokątów przypadających na jeden proces
    int local_n = n / size;
    int remainder = n % size;

    // Obliczanie zakresu dla każdego procesu
    int start_i = rank * local_n;
    if (rank < remainder) {
        local_n++;
        start_i += rank;
    }
    else {
        start_i += remainder;
    }
    int end_i = start_i + local_n;

    // Pomiar czasu
    double start_time = MPI_Wtime();

    // Obliczanie sumy częściowej
    for (int i = start_i; i < end_i; i++) {
        double x = a + i * h;
        local_sum += f(x) * h;
    }

    // Redukcja - sumowanie wszystkich częściowych wyników
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Pomiar czasu zakończenia
    double end_time = MPI_Wtime();
    double duration = end_time - start_time;

    // Proces główny wyświetla wynik
    if (rank == 0) {
        // Ustawienie większej precyzji wyświetlania
        std::cout << std::setprecision(15) << std::fixed;

        std::cout << "Przyblizona wartosc calki: " << total_sum << std::endl;
        std::cout << "Dokladna wartosc calki sin(x) od 0 do PI: 2.000000000000000" << std::endl;
        std::cout << "Blad: " << std::abs(total_sum - 2.0) << std::endl;
        std::cout << "Czas wykonania: " << duration << " sekundy" << std::endl;
    }

    MPI_Finalize();
    return 0;
}