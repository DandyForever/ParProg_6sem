#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char* argv[]) {
	int rank = -1, num = 0, recv = -1;
	MPI_Status status;
	MPI_Init (&argc, &argv);
	
	MPI_Comm_size (MPI_COMM_WORLD, &num);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	

	if (rank == 0) {
		printf ("%d\n", rank);
		fflush (stdout);
		MPI_Send (&rank, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
	} else if (rank != num - 1) {
		MPI_Recv (&recv, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		//printf ("I'm %d and I recv %d\n", rank, recv);
		printf ("%d\n", rank);
		fflush (stdout);
		MPI_Send (&rank, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
	} else {
		MPI_Recv (&recv, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		//printf ("I'm %d and I recv %d\n", rank, recv);
		printf ("%d\n", rank);
		fflush (stdout);
	}

	MPI_Finalize ();
	return 0;
}
