#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <malloc.h>

typedef enum {
  OK = 0,
  FAIL = 1
} status_t;

typedef struct {
  int start;
  int end;
} task_t;

int main(int argc, char* argv[])
{
  status_t status = FAIL;
  int task_size = 0;
  int length = 0;
  task_t localTask = {};
  int rank = 0;
  int size = 0;
  double startTime = 0;
  double endTime = 0;
  int i = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    // Root branch
    //
    // Check arguments
    if (argc != 3)
    {
      printf("Usage:\n required 2 arguments input and output file name\n");
      // Exit with fail status
      status = FAIL;
      MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Finalize();
      return 0;
    }

    // Send status of arguments
    status = OK;
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);

	startTime = MPI_Wtime();

	FILE* fp;
	fp = fopen(argv[1], "r");
	fscanf (fp, "%d", &length);
	task_size = length / 9 / (size - 1);
	MPI_Bcast(&task_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int * first = malloc (sizeof (int) * task_size * size);
	int * second = malloc (sizeof (int) * task_size * size);
	for (i = task_size; i < task_size * size; i++)
		fscanf (fp, "%9d", first + i);
	for (i = task_size; i < task_size * size; i++)
		fscanf (fp, "%9d", second + i);
	fclose (fp);

	int * num1 = malloc (sizeof(int) * task_size);
	int * num2 = malloc (sizeof(int) * task_size);

	MPI_Scatter (first, task_size, MPI_INT, num1, task_size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter (second, task_size, MPI_INT, num2, task_size, MPI_INT, 0, MPI_COMM_WORLD);

	int recv = 0;
	MPI_Status st;
	MPI_Recv (&recv, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &st);

	MPI_Gather (num1, task_size, MPI_INT, first, task_size, MPI_INT, 0, MPI_COMM_WORLD);

	fp = fopen (argv[2], "w");
	if (recv)
		fprintf (fp, "1");
	for (i = task_size; i < task_size * size; i++)
		fprintf (fp, "%.9d", first[i]);
	fclose (fp);
	
	free (num1);
	free (num2);
	free (first);
	free (second);
  }
  else
  {
    // Slave branch
    //
    // Check status
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (status == FAIL)
    {
      MPI_Finalize();
      return 0;
    }

	MPI_Bcast(&task_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
   
	int * num1 = malloc (sizeof(int) * task_size);
	int * num2 = malloc (sizeof(int) * task_size);
	int * result1 = malloc (sizeof(int) * task_size);
	int * result2 = malloc (sizeof(int) * task_size);

	MPI_Scatter (NULL, 0, 0, num1, task_size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter (NULL, 0, 0, num2, task_size, MPI_INT, 0, MPI_COMM_WORLD);

	int spec = 1;
	int sign = 0;
	
	for (i = task_size - 1; i >= 0; i--){
		if (spec) {
			result1[i] = num1[i] + num2[i] + sign;
			result2[i] = num1[i] + num2[i] + 1 + sign;
		} else {
			result1[i] = num1[i] + num2[i] + sign;
			result2[i] = result1[i];
		}
		if (result1[i] >= 1000000000) {
			result1[i] -= 1000000000;
			result2[i] -= 1000000000;
			sign = 1;
			spec = 0;
		} else if (result2[i] >= 1000000000) {
			result2[i] -= 1000000000;
			spec = 1;
			sign = 0;
		} else {
			spec = 0;
			sign = 0;
		}
	}
	int recv = 0;
	MPI_Status st;

	if (rank != size - 1) {
		MPI_Recv (&recv, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,&st);
	}
	if (spec && recv)
		sign = 1;
	MPI_Send (&sign, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

	if (recv == 1)
		MPI_Gather (result2, task_size, MPI_INT, num1, task_size, MPI_INT, 0, MPI_COMM_WORLD);
	else
		MPI_Gather (result1, task_size, MPI_INT, num1, task_size, MPI_INT, 0, MPI_COMM_WORLD);


	free (num1);
	free (num2);
	free (result1);
	free (result2);
  }


  // Finish
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
  {
    endTime = MPI_Wtime();
    printf("[TIME RES] %lf\n", endTime - startTime);
  }

  MPI_Finalize();
  return 0;
}

