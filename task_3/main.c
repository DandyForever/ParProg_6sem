#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define M_PI 3.14159265358979323846

int main (int argc, char* argv[]) {
	int rank = -1, num = 0;
	MPI_Status status;
	MPI_Init (&argc, &argv);

	MPI_Comm_size (MPI_COMM_WORLD, &num);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	double time_begin = 0;
	
	if (argc != 2)
		return 0;

	int task_buffer[1000];
	int my_task[2] = {};
	if (rank == 0) {
		int length = atoi(argv[1]);
		int task = length / num, given_task = 0;;
		for (int i = 0; i < num; i++) {
			task_buffer[2 * i] = task;
			given_task += task;
		}
		int i = 1;
		while (given_task < length) {
			task_buffer[2 * i]++;
			given_task++;
			i++;
		}
		task_buffer[1] = length;
		for (int i = 1; i < num; i++) {
			task_buffer[2 * i + 1] = task_buffer[2 * (i - 1) + 1] - task_buffer[2 * (i - 1)];
		}
	}
	
	time_begin = MPI_Wtime ();
	MPI_Scatter (task_buffer, 2, MPI_INT, my_task, 2, MPI_INT, 0, MPI_COMM_WORLD);
	double result = 0.;
	for (int i = my_task[1]; i > my_task[1] - my_task[0]; i--) {
		result += (1. / i) * (1. / i);
	}
	double sum = 0.;
	MPI_Reduce (&result, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		sum = sum * 6 / (M_PI * M_PI);
		printf ("[ans] = %lf, [time] = %.6lf\n", sum, MPI_Wtime () - time_begin); 
	}
	MPI_Finalize ();
	return 0;
}
