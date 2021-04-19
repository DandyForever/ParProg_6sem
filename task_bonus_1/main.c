#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void my_Bcast (void* buf, int count, MPI_Datatype type, int root, MPI_Comm comm);
void my_Reduce (void* sendbuf, void* recvbuf, int count, MPI_Op op, int root, MPI_Comm comm);
void my_Scatter (void* sbuf, int scount, MPI_Datatype stype, void* rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm);
void my_Gather (void* sbuf, int scount, MPI_Datatype stype, void* rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm);
void testing (int recv, int rank, int num);

int main (int argc, char* argv[]) {
	int rank = -1, num = 0, recv = -1;
	MPI_Status status;
	MPI_Init (&argc, &argv);
	
	MPI_Comm_size (MPI_COMM_WORLD, &num);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	recv = rank;
	//testing(recv, rank, num);

	double time = 0, time_beg = 0, time_end = 0;
	int buf[10000] = {0};
	int buf_[10000] = {0};
	int iter = 1000000;
{
	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		MPI_Bcast (buf, 10000, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}	

	if (rank == 0)
		printf ("MPI_Broadcast time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}

{	
	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		my_Bcast (buf, 10000, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}	

	if (rank == 0)
		printf ("my_Broadcast time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}

{
	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		MPI_Reduce (buf_, buf, 1000, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}

	if (rank == 0)
		printf ("MPI_Reduce time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}
{
	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		my_Reduce (buf_, buf, 1000, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}

	if (rank == 0)
		printf ("my_Reduce time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}
{
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
		printf ("MPI_Scatter time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}
{
	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		my_Scatter (buf, 1000, MPI_INT, buf, 1000, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}	
	
	if (rank == 0)
		printf ("my_Scatter time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}

{
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
		printf ("MPI_Gather time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}

{
	time = 0;
	for (int i = 0; i < iter; i++) {
		MPI_Barrier (MPI_COMM_WORLD);
		time_beg = MPI_Wtime ();
		my_Gather (buf, 1000, MPI_INT, buf, 1000, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier (MPI_COMM_WORLD);
		time_end = MPI_Wtime ();
		time += time_end - time_beg;
	}	
	
	if (rank == 0)
		printf ("my_Gather time %.6lf+-%.6f\n", time / iter, 2 * MPI_Wtick ());
}

	MPI_Finalize ();
	return 0;
}

void my_Bcast (void* buf, int count, MPI_Datatype type, int root, MPI_Comm comm) {
	int rank = -1, num = 0;
	MPI_Status status;

	MPI_Comm_size (comm, &num);
	MPI_Comm_rank (comm, &rank);

	if (root >= num) {
		if (rank == 0)
			printf ("invalid number of root process in Bcast\n");
		return;
	}

	if (rank == root) {
		for (int i = 0; i < num; i++) {
			if (i != root)
				MPI_Send(buf, count, type, i, 0, comm);
		}
	} else {
		MPI_Recv(buf, count, type, root, 0, comm, &status);
	}
}

void my_Reduce (void* sendbuf, void* recvbuf, int count, MPI_Op op, int root, MPI_Comm comm) {
	int rank = -1, num = 0;
	MPI_Status status;

	MPI_Comm_size (comm, &num);
	MPI_Comm_rank (comm, &rank);

	if (root >= num) {
		if (rank == 0)
			printf ("invalid number of root process in Reduce\n");
		return;
	}
	MPI_Send(sendbuf, count, MPI_INT, root, 0, comm);

	if (rank == root) {
		int* buffer = (int*) malloc(num * count * sizeof(int));
		int* recv = (int*)recvbuf;
		//int* buffer = (int*)sendbuf;
		//for (int i = 0; i < count; i++) {
		//	((int*)recvbuf)[i] = ((int*)sendbuf)[i];
		//}
		for (int i = 0; i < num; i++) {
			MPI_Recv(buffer + i * count, count, MPI_INT, i, 0, comm, &status);
		}
		for (int i = 0; i < count; i++) {
			recv[i] = buffer[i];
		}
		for (int i = 1; i < num; i++) {
			int j = i * count;
			for (int l = 0; l < count; l++, j++) {
				if (op == MPI_SUM)
					recv[l] += buffer[j];
				else if (op == MPI_MAX)
					recv[l] = (buffer[j] > recv[l]) ? buffer[j] : recv[l];
				else if (op == MPI_MIN)
					recv[l] = (buffer[j] < recv[l]) ? buffer[j] : recv[l];
				else if (op == MPI_PROD)
					recv[l] *= buffer[j];
				else if (op == MPI_LAND)
					recv[l] = recv[l] && buffer[j];
				else if (op == MPI_BAND)
					recv[l] &= buffer[j];
				else if (op == MPI_LOR)
					recv[l] = recv[l] || buffer[j];
				else if (op == MPI_BOR)
					recv[l] |= buffer[j];
				else if (op == MPI_LXOR)
					recv[l] = !recv[l] && buffer[j] || recv[l] && !buffer[j];
				else if (op == MPI_BXOR)
					recv[l] ^= buffer[j];
			}
		}
		free (buffer);
	}
}

void my_Scatter (void* sbuf, int scount, MPI_Datatype stype, void* rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm) {
	int rank = -1, num = 0;
	MPI_Status status;

	MPI_Comm_size (comm, &num);
	MPI_Comm_rank (comm, &rank);

	if (root >= num) {
		if (rank == 0)
			printf ("invalid number of root process in Scatter\n");
		return;
	}
	if (scount != rcount) {
		if (rank == 0)
			printf ("Mismatch between scount and rcount in Scatter\n");
		return;
	}

	if (rank == root) {
		for (int i = 0; i < num; i++) {
			int size = 0;
			MPI_Type_size (stype, &size);
			MPI_Send (sbuf + i * scount * size, scount, stype, i, 0, comm);
		}
	}

	MPI_Recv (rbuf, rcount, rtype, root, 0, comm, &status);
}

void my_Gather (void* sbuf, int scount, MPI_Datatype stype, void* rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm) {
	int rank = -1, num = 0;
	MPI_Status status;

	MPI_Comm_size (comm, &num);
	MPI_Comm_rank (comm, &rank);

	if (root >= num) {
		if (rank == 0)
			printf ("invalid number of root process in Gather\n");
		return;
	}
	if (scount != rcount) {
		if (rank == 0)
			printf ("Mismatch between scount and rcount in Gather\n");
		return;
	}

	MPI_Send (sbuf, scount, stype, root, 0, comm);
	
	if (rank == root) {
		for (int i = 0; i < num; i++) {
			int size = 0;
			MPI_Type_size (stype, &size);
			MPI_Recv (rbuf + i * rcount * size, rcount, rtype, i, 0, comm, &status);
		}
	}
}
void testing (int recv, int rank, int num){	
	for (int i = 0; i < 1000; i++) {
		MPI_Bcast (&recv, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	recv = rank;
	//printf ("before reduce rank %d recv %d\n", rank, recv);
	my_Reduce (&rank, &recv, 1, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("sum %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_MAX, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("max %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_MIN, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("min %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_PROD, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("prod %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_LAND, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("land %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_BAND, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("band %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_LOR, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("lor %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_BOR, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("bor %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_LXOR, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("lxor %d\n", recv);
	recv = rank;	
	my_Reduce (&rank, &recv, 1, MPI_BXOR, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf ("bxor %d\n", recv);

	int buffer[10] = {};
	for (int i = 0; i < num; i++) {
		buffer[i] = i*i;
		if (rank == 0)
			printf ("to %d - %d\n", i, buffer[i]);
	}
	my_Scatter (buffer, 1, MPI_INT, &recv, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf ("[%d] recv=%d\n", rank, recv);
	for (int i = 0; i < num; i++) {
		buffer[i] = 0;
	}
	my_Gather (&recv, 1, MPI_INT, buffer, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0)
		for (int i = 0; i < num; i++) {
			printf ("[%d] b[i]=%d\n", rank, buffer[i]);
		}
}
