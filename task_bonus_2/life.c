#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define ROWS 1000
#define COLS 1000

int main (int argc, char* argv[]) {
	int rank = 0, size = 0, width = 0, height = 0;
	srand(time(NULL));
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (argc != 3 || (width = atoi(argv[1])) < 1 || (height = atoi(argv[2])) < 1) {
		if (rank == 0)
			printf ("Wrong arguments\n");
		MPI_Finalize();
	}

	char frame[ROWS][COLS], temp[ROWS][COLS];
	int h = height / size, w = width;
	for (int i = 1; i <= h; i++) {
		for (int j = 1; j <= w; j++) {
			if (rand() % 2)
				frame[i][j] = '#';
			else
				frame[i][j] = ' ';
		}
	}

	if (height % size > rank) {
		h++;
		for (int i = 1; i <= w; i++) {
			if (rand() % 2)
				frame[h][i] = '#';
			else
				frame[h][i] = ' ';
		}
	}

	for (int i = 0; i <= h + 1; i++) {
		frame[i][0] = ' ';
		frame[i][w + 1] = ' ';
	}
	if (rank == 0) {
		for (int i = 0; i <= w + 1; i++)
			frame[0][i] = ' ';
	}
	if (rank == size - 1) {
		for (int i = 0; i <= w + 1; i++)
			frame[h + 1][i] = ' ';
	}

	for (int i = 0; i <= h + 1; i++) {
		for (int j = 0; j <= w + 1; j++) {
			temp[i][j] = frame[i][j];
		}
	}


	int recv = 0;
	char toPrint[ROWS*COLS];
	int ind = 0;
	MPI_Status status;
	double start = MPI_Wtime();
	double end = 0.;
	int number = 0, prev = 0;
	char fromPrev[COLS], fromNext[COLS], toPrev[COLS], toNext[COLS];
	do {
		for (int i = 1; i <= h; i++) {
			for (int j = 1; j <= w; j++) {
				toPrint[ind++] = frame[i][j];
			}
			toPrint[ind++] = '\n';
		}
		toPrint[ind] = '\0';
		MPI_Barrier (MPI_COMM_WORLD);
		if (rank != 0)
			MPI_Recv (&recv, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		else {
			usleep (10000);
			system ("clear");
			end = MPI_Wtime();
			if (end - start > 1) {
				printf ("FPS: %d\n", number);
				prev = number;
				number = 0;
				start = end;
			} else {
				printf ("FPS: %d\n", prev);
			}
		}
		write (1, toPrint, ind);
		fflush(NULL);
		number++;
		ind = 0;
		if (rank != size - 1)
			MPI_Send (&rank, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);


		for (int i = 0; i <= w + 1; i++) {
			toPrev[i] = frame[1][i];
			toNext[i] = frame[h][i];
			fromNext[i] = ' ';
			fromPrev[i] = ' ';
		}
		if (rank != 0)
			MPI_Recv (fromPrev, w + 2, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &status);
		if (rank != size - 1) {
			MPI_Send (toPrev, w + 2, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
			MPI_Recv (fromNext, w + 2, MPI_CHAR, rank + 1, 2, MPI_COMM_WORLD, &status);
		}
		if (rank != 0)
			MPI_Send (toNext, w + 2, MPI_CHAR, rank - 1, 2, MPI_COMM_WORLD);
	 
		for (int i = 0; i <= w + 1; i++) {
			frame[0][i] = fromPrev[i];
			frame[h + 1][i] = fromNext[i];
		}

		int count = 0;
		for (int i = 1; i <= h; i++) {
			for (int j = 1; j <= w; j++) {
				count = 0;
				if (frame[i - 1][j] == '#') count++;
				if (frame[i + 1][j] == '#') count++;
				if (frame[i][j - 1] == '#') count++;
				if (frame[i][j + 1] == '#') count++;
				if (frame[i - 1][j - 1] == '#') count++;
				if (frame[i - 1][j + 1] == '#') count++;
				if (frame[i + 1][j - 1] == '#') count++;
				if (frame[i + 1][j + 1] == '#') count++;
				if (frame[i][j] == ' ' && count == 3) temp[i][j] = '#';
				else if (frame[i][j] == '#' && !(count == 2 || count == 3)) temp[i][j] = ' ';
			}
		}
		for (int i = 1; i <= h; i++) {
			for (int j = 1; j <= w; j++) {
				frame[i][j] = temp[i][j];
			}
		}
	} while(1);

	MPI_Finalize();
	return 0;
}
