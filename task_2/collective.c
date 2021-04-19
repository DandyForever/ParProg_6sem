#include <stdio.h>
#include <mpi.h>

int main (int argc, char* argv[]) {
	int rank = -1, num = 0, recv = -1;
	MPI_Status status;
	MPI_Init (&argc, &argv);
	
	MPI_Comm_size (MPI_COMM_WORLD, &num);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);		

	recv = rank;

	double time = 0, time_beg = 0, time_end = 0;
	int buf[10000] = {0};
	int buf_[10000] = {0};
	int iter = 1000000;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		MPI_Bcast (buf, 10000, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}	

	if (rank == 0)
		printf ("Broadcast time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());

	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		MPI_Reduce (buf_, buf, 10000, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}

	if (rank == 0)
		printf ("Reduce time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());

	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		MPI_Scatter (buf, 1000, MPI_INT, buf, 1000, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}	
	
	if (rank == 0)
		printf ("Scatter time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());

	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		MPI_Gather (buf, 1000, MPI_INT, buf, 1000, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}	
	
	if (rank == 0)
		printf ("Gather time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());


	MPI_Finalize ();
	return 0;
}
