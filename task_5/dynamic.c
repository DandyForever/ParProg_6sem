#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <malloc.h>

typedef enum {
  OK = 0,
  FAIL = 1
} status_t;

typedef struct {
	int index;
	int spec;
	int next1;
	int next2;
	int size;
	int first[100];
	int second[100];
} task_t;

int main(int argc, char* argv[])
{
  status_t argStatus = FAIL;
  MPI_Status status = {};
  int tasksNumber = 0;
  int blockSize = 0;
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
    if (argc != 4 || (blockSize = atoi(argv[1])) < 1)
    {
      printf("Usage:\n required 2 arguments (> 0)\
              \n for tasks number and block size\n");
      // Exit with fail status
      argStatus = FAIL;
      MPI_Bcast(&argStatus, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Finalize();
      return 0;
    }
	
	argStatus = OK;
    MPI_Bcast(&argStatus, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
    startTime = MPI_Wtime();
    
	FILE* fp;
	int length = 0;
	fp = fopen(argv[2], "r");
	fscanf (fp, "%d", &length);
	int task_size = length / 9;
	
	int * first = malloc (sizeof (int) * task_size);
	int * second = malloc (sizeof (int) * task_size);
	for (i = 0; i < task_size; i++)
		fscanf (fp, "%9d", first + i);
	for (i = 0; i < task_size; i++)
		fscanf (fp, "%9d", second + i);
	fclose (fp);
   
	task_t* taskShedule = malloc ((task_size / blockSize + 1) * sizeof(task_t));
	int blockNumber = task_size / blockSize;
	int rest = task_size % blockSize;

	for (i = 0; i < blockNumber; i++) {
		taskShedule[i].index = i;
		taskShedule[i].spec = 0;
		taskShedule[i].next1 = 0;
		taskShedule[i].next2 = 0;
		taskShedule[i].size = blockSize;
		for (int j = 0; j < blockSize; j++) {
			taskShedule[i].first[j] = first[i * blockSize + j];
			taskShedule[i].second[j] = second[i * blockSize + j];
		}
	}

	if (rest != 0) {
		taskShedule[blockNumber].index = blockNumber;
		taskShedule[blockNumber].spec = 0;
		taskShedule[blockNumber].next1 = 0;
		taskShedule[blockNumber].next2 = 0;
		taskShedule[blockNumber].size = rest;
		int index = blockNumber * blockSize;
		for (int j = 0; j < rest; j++) {
			taskShedule[blockNumber].first[j] = first[index + j];
			taskShedule[blockNumber].second[j] = second[index + j];
		}
		blockNumber++;
	}

    // Send blockss for work begin
    int blockPtr = 0;
    task_t buf = {};
    int slave = 0;

    for (blockPtr = 0; blockPtr < blockNumber; blockPtr++)
    {
      // Find slave
      MPI_Recv(&buf, sizeof(task_t) / sizeof(int), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      slave = status.MPI_SOURCE;
	  if (buf.index != -1){
	  	taskShedule[buf.index] = buf;
	  }
      // Send block
      MPI_Send(&taskShedule[blockPtr], sizeof(task_t) / sizeof(int), MPI_INT, slave, OK, MPI_COMM_WORLD);
    }
	
    // End calc
    for (i = 1; i < size; i++)
    {
      MPI_Recv(&buf, sizeof(task_t) / sizeof(int), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
       if (buf.index != -1){
		   taskShedule[buf.index] = buf;
	   }
	  slave = status.MPI_SOURCE;
      MPI_Send(&taskShedule[0], sizeof(task_t) / sizeof(int), MPI_INT, slave, FAIL, MPI_COMM_WORLD);
    }

	int prev = 0, ind = 0;
	fp = fopen(argv[3], "w");

	for (i = blockNumber - 1; i >= 0; i--) {
		for (int j = taskShedule[i].size - 1; j >= 0; j--) {
			if (taskShedule[i].spec) {
				if (prev) {
					first[ind] = taskShedule[i].second[j];
					prev = taskShedule[i].next2;
				} else {
					first[ind] = taskShedule[i].first[j];
					prev = taskShedule[i].next1;
				}
			}
			else {
				if (j == taskShedule[i].size - 1)
					first[ind] = taskShedule[i].first[j] + prev;
				else
					first[ind] = taskShedule[i].first[j];
				prev = taskShedule[i].next1;
			}
		ind++;
		}
	}

	if (prev)
		fprintf (fp, "1");

	for (i = ind - 1; i >= 0; i--) 
		fprintf (fp, "%.9d", first[i]);

	fclose (fp);

	free (first);
	free (second);
	free (taskShedule);

  }
  else
  {
    // Slave branch
    //
    // Check status
    MPI_Bcast(&argStatus, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (argStatus == FAIL)
    {
      MPI_Finalize();
      return 0;
    }
    task_t localResult = {};
	localResult.index = -1;

    while (1)
    {
      // Ready for tasks
      MPI_Send(&localResult, sizeof(task_t) / sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
      // Get block
      MPI_Recv(&localTask, sizeof(task_t) / sizeof(int), MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_TAG == OK)
      {
		int to_next1 = 0;
		int to_next2 = 0;
		localResult.size = localTask.size;
		localResult.index = localTask.index;
		if (localTask.first[localTask.size - 1] + localTask.second[localTask.size - 1] == 999999999) {
			localResult.first[localTask.size - 1] = 999999999;
			localResult.second[localTask.size - 1] = 0;
			to_next2 = 1;
			localResult.spec = 1;
		} else {
			localResult.first[localTask.size - 1] = localTask.first[localTask.size - 1] + localTask.second[localTask.size - 1];
			if (localResult.first[localTask.size - 1] >= 1000000000) {
				localResult.first[localTask.size - 1] -= 1000000000;
				to_next1 = 1;
			}
			localResult.spec = 0;
		}
		for (int i = localTask.size - 2; i >= 0; i--) {
				localResult.first[i] = localTask.first[i] + localTask.second[i] + to_next1;
				if (localResult.first[i] >= 1000000000) {
					localResult.first[i] -= 1000000000;
					to_next1 = 1;
				} else {
					to_next1 = 0;
				}
			if (localResult.spec) {
				localResult.second[i] = localTask.first[i] + localTask.second[i] + to_next2;
				if (localResult.second[i] >= 1000000000) {
					localResult.second[i] -= 1000000000;
					to_next2 = 1;
				} else {
					to_next2 = 0;
				}
			}
		}
		localResult.next1 = to_next1;
		localResult.next2 = to_next2;
      } else {
        break;
      }
    }
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

