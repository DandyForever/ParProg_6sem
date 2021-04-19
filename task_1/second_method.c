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
		for (int i = 1; i < num; i++) {
			MPI_Send (&rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Recv (&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		}
	} else {
		MPI_Recv (&recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		//printf ("I'm %d and I recv %d\n", rank, recv);
		printf ("%d\n", rank);
		fflush (stdout);
		MPI_Send (&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize ();
	return 0;
}
