#include "mpi.h"
#include <cstdio>
#include <iostream>
#include <time.h>
#include <vector>

void mainProcess(int size) {
    srand(time(NULL));

    // Pobieranie rozmiaru wektor�w od u�ytkownika
    int vectorSize = 0;
    std::cout << "Podaj rozmiar wektor�w: ";
    std::cin >> vectorSize;

    if (vectorSize <= 0) {
        std::cout << "Nieprawid�owy rozmiar wektora. Ustawiam domy�lny rozmiar 10.\n";
        vectorSize = 10;
    }

    // Obliczanie ile element�w przypada na ka�dy proces
    std::vector<int> elementsPerProcess(size);
    std::vector<int> startIndices(size);

    // Podstawowy przydzia� (minimalna liczba element�w dla ka�dego procesu)
    int baseElements = vectorSize / (size - 1);
    int remainingElements = vectorSize % (size - 1);

    // Proces g��wny (0) nie wykonuje oblicze�
    elementsPerProcess[0] = 0;
    startIndices[0] = 0;

    // Przydzielanie element�w dla proces�w roboczych (1 do size-1)
    for (int i = 1; i < size; i++) {
        elementsPerProcess[i] = baseElements;
        if (i <= remainingElements) {
            elementsPerProcess[i]++;
        }

        // Obliczanie indeksu pocz�tkowego dla ka�dego procesu
        if (i == 1) {
            startIndices[i] = 0;
        }
        else {
            startIndices[i] = startIndices[i - 1] + elementsPerProcess[i - 1];
        }
    }

    // Alokujemy wektory o rozmiarze podanym przez u�ytkownika
    unsigned int* va = new unsigned int[vectorSize];
    unsigned int* vb = new unsigned int[vectorSize];
    unsigned int* vc = new unsigned int[vectorSize];

    // Wype�niamy a i b losowymi danymi, a vc zerujemy
    for (int i = 0; i < vectorSize; i++) {
        va[i] = rand() % 10;
        vb[i] = rand() % 10;
        vc[i] = 0;
    }

    // Wysy�amy informacje o rozmiarze wektora do wszystkich proces�w
    MPI_Bcast(&vectorSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Wysy�amy tablice z informacj� o liczbie element�w dla ka�dego procesu
    MPI_Bcast(elementsPerProcess.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Wysy�amy tablice z indeksami pocz�tkowymi dla ka�dego procesu
    MPI_Bcast(startIndices.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcastujemy wektor a do pozosta�ych proces�w
    MPI_Bcast(va, vectorSize, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // Broadcastujemy wektor b do pozosta�ych proces�w
    MPI_Bcast(vb, vectorSize, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // Odbieramy wyniki od proces�w roboczych
    MPI_Request* requests = new MPI_Request[size - 1];
    MPI_Status* statuses = new MPI_Status[size - 1];

    for (int i = 1; i < size; i++) {
        MPI_Irecv(vc + startIndices[i], elementsPerProcess[i], MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &requests[i - 1]);
    }

    MPI_Waitall(size - 1, requests, statuses);

    // Wypisujemy wyniki
    std::cout << "Wektor A: ";
    for (int i = 0; i < vectorSize; i++) std::cout << va[i] << " ";
    std::cout << "\n";

    std::cout << "Wektor B: ";
    for (int i = 0; i < vectorSize; i++) std::cout << vb[i] << " ";
    std::cout << "\n";

    std::cout << "Wynik (A+B): ";
    for (int i = 0; i < vectorSize; i++) std::cout << vc[i] << " ";
    std::cout << "\n";

    // Zwalniamy pami��
    delete[] va;
    delete[] vb;
    delete[] vc;
    delete[] requests;
    delete[] statuses;
}

void workerProcess(int id, int size) {
    // Odbieramy rozmiar wektor�w
    int vectorSize;
    MPI_Bcast(&vectorSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Odbieramy informacje o liczbie element�w dla ka�dego procesu
    std::vector<int> elementsPerProcess(size);
    MPI_Bcast(elementsPerProcess.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Odbieramy informacje o indeksach pocz�tkowych
    std::vector<int> startIndices(size);
    MPI_Bcast(startIndices.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Alokujemy bufory na moj� cz�� zadania
    int myElements = elementsPerProcess[id];
    unsigned int* myResult = new unsigned int[myElements];

    // Alokujemy miejsce na wektory a oraz b
    unsigned int* va = new unsigned int[vectorSize];
    unsigned int* vb = new unsigned int[vectorSize];

    // Nas�uchujemy bcasta wektora a
    MPI_Bcast(va, vectorSize, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // Nas�uchujemy bcasta wektora b
    MPI_Bcast(vb, vectorSize, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // Liczymy sum� dla naszego fragmentu
    int startIndex = startIndices[id];
    for (int i = 0; i < myElements; i++) {
        myResult[i] = va[startIndex + i] + vb[startIndex + i];
    }

    // Odsy�amy wynik
    MPI_Send(myResult, myElements, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);

    // Zwalniamy pami��
    delete[] myResult;
    delete[] va;
    delete[] vb;
}

int main()
{
    int PID, PCOUNT;

    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &PID);
    MPI_Comm_size(MPI_COMM_WORLD, &PCOUNT);

    if (PID == 0) { // Jestem procesem g��wnym
        mainProcess(PCOUNT);
    }
    else { // Jestem procesem roboczym
        workerProcess(PID, PCOUNT);
    }

    MPI_Finalize();

    return 0;
}