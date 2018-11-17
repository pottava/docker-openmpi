#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int  rank, size, len;
    char name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Get_processor_name( name, &len );
    name[len] = '\0';
    printf( "Hello, world, I am %d of %d from %s\n", rank, size, name );
    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();

    return 0;
}
