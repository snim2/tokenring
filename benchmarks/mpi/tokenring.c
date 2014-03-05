/* Token ring network, used to estimate the time taken to pass a
 * message in an MPI network.
 *
 * Sarah Mount, November 2011
 */


#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>


#define MAXCIRCUITS 100

int main(int argc, char **argv) {
    int rank, size, dest, src;
    int token = 0;

    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0 && token == 0) {
	dest = 1;
	MPI_Send(&token, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
    }
    while (token < (size * MAXCIRCUITS)) {
	// The modulo of a negative number is undefined in the C specification!
	src = ((rank + size) - 1) % size;
	dest = (rank + 1) % size;
	MPI_Recv(&token, 1, MPI_INT, src, 1, MPI_COMM_WORLD,  &status);
	token++;
	MPI_Send(&token, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
