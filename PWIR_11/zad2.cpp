#include "mpi.h"
#include <cstdio>
#include <iostream>
#include <time.h>

void mainProcess(int size) {
	srand(time(NULL));

	//alokujemy wektory o rozmiarze(8*(ilosc procesów-1))
	unsigned int* va = new unsigned int[8 * (size - 1)];
	unsigned int* vb = new unsigned int[8 * (size - 1)];
	unsigned int* vc = new unsigned int[8 * (size - 1)];

	//wype³niamy a i b losowymi danymi, a vc zerujemy
	for (unsigned int i = 0; i < 8 * (size - 1); i++) {
		va[i] = rand() % 10;
		vb[i] = rand() % 10;
		vc[i] = 0;
	}

	//broadcastujemy wektor a do pozosta³ych procesów
	MPI_Bcast(va, 8 * (size - 1), MPI_UNSIGNED, 0, MPI_COMM_WORLD);

	//broadcastujemy wektor b do pozosta³ych procesów
	MPI_Bcast(vb, 8 * (size - 1), MPI_UNSIGNED, 0, MPI_COMM_WORLD);

	//odpalamy nas³uch
	MPI_Request* requests = new MPI_Request[size - 1];
	MPI_Status* statuses = new MPI_Status[size - 1];
	for (unsigned int i = 0; i < size - 1; i++) {
		MPI_Irecv(vc + i * 8, 8, MPI_UNSIGNED, i + 1, 0, MPI_COMM_WORLD, &requests[i]);
	}
	MPI_Waitall(size - 1, requests, statuses);

	//wypisujemy wyniki
	for (unsigned int i = 0; i < (8 * (size - 1)); i++) printf("%d\t", va[i]);
	printf("\r\n");
	for (unsigned int i = 0; i < (8 * (size - 1)); i++) printf("%d\t", vb[i]);
	printf("\r\n");
	for (unsigned int i = 0; i < (8 * (size - 1)); i++) printf("%d\t", vc[i]);
	printf("\r\n");

	//zwalniamy pamiêæ
	delete[] va;
	delete[] vb;
	delete[] vc;
	delete[] requests;
	delete[] statuses;
}

void workerProcess(int id, int size) {
	//alokujemy buffor na moj¹ czêœæ zadania
	unsigned int* v = new unsigned int[8];

	//alokujemny miejsce na wektor a oraz b
	unsigned int* va = new unsigned int[8 * (size - 1)];
	unsigned int* vb = new unsigned int[8 * (size - 1)];

	//nas³uchujemy bcasta wektora a
	MPI_Bcast(va, 8 * (size - 1), MPI_UNSIGNED, 0, MPI_COMM_WORLD);

	//nas³uchujemy bcasta wektora b
	MPI_Bcast(vb, 8 * (size - 1), MPI_UNSIGNED, 0, MPI_COMM_WORLD);

	//liczymy sumê
	for (unsigned int i = 0; i < 8; i++) {
		v[i] = va[(id - 1) * 8 + i] + vb[(id - 1) * 8 + i];
	}

	//odsy³amy wynik
	MPI_Send(v, 8, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);

	//zwalniamy pamiêc
	delete[] v;
	delete[] va;
	delete[] vb;
}

int main()
{
	int PID, PCOUNT;

	MPI_Init(NULL, NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &PID);
	MPI_Comm_size(MPI_COMM_WORLD, &PCOUNT);

	if (PID == 0) { //jestem procesem g³ównym
		mainProcess(PCOUNT);
	}
	else { //jestem procesem roboczym
		workerProcess(PID, PCOUNT);
	}

	MPI_Finalize();

	return 0;
}

