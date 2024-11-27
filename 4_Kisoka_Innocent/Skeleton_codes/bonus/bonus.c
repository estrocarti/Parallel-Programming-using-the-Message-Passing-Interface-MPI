#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SUBDOMAIN 6
#define DOMAINSIZE (SUBDOMAIN+2)

int main(int argc, char *argv[])
{
    int rank, size, i, j, dims[2], periods[2], rank_top, rank_bottom, rank_left, rank_right;
    double data[DOMAINSIZE*DOMAINSIZE];
    MPI_Request requests[8];
    MPI_Status statuses[8];
    MPI_Comm comm_cart;
    MPI_Datatype column_type;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 16) {
        printf("Please run this with 16 processors\n");
        MPI_Finalize();
        exit(1);
    }

  
    // initialize the domain
    for (i=0; i<DOMAINSIZE*DOMAINSIZE; i++) {
        data[i]=rank;
    }

    // TODO: set the dimensions of the processor grid and periodic boundaries in both dimensions
    // Set up cartesian topology
    dims[0] = dims[1] = 4;
    periods[0] = periods[1] = 1;

    // TODO: Create a Cartesian communicator (4*4) with periodic boundaries (we do not allow
    // the reordering of ranks) and use it to find your neighboring
    // ranks in all dimensions in a cyclic manner.
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_cart);

    // TODO: find your top/bottom/left/right neighbor using the new communicator, see MPI_Cart_shift()
    // rank_top, rank_bottom
    // rank_left, rank_right
    MPI_Cart_shift(comm_cart, 0, 1, &rank_top, &rank_bottom);
    MPI_Cart_shift(comm_cart, 1, 1, &rank_left, &rank_right);

    //  TODO: create derived datatype data_ghost, create a datatype for sending the column, see MPI_Type_vector() and MPI_Type_commit()
    // data_ghost
    MPI_Type_vector(DOMAINSIZE, 1, DOMAINSIZE, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);

    //  TODO: ghost cell exchange with the neighbouring cells in all directions
    //  use MPI_Irecv(), MPI_Send(), MPI_Wait() or other viable alternatives
    // Exchange ghost cells : Top, and Bottom
    
  

    MPI_Irecv(&data[1], SUBDOMAIN, MPI_DOUBLE, rank_top, 0, comm_cart, &requests[0]);
    MPI_Irecv(&data[DOMAINSIZE*DOMAINSIZE-SUBDOMAIN-1], SUBDOMAIN, MPI_DOUBLE, rank_bottom, 1, comm_cart, &requests[1]);
    MPI_Isend(&data[DOMAINSIZE+2], SUBDOMAIN, MPI_DOUBLE, rank_top, 1, comm_cart, &requests[2]);
    MPI_Isend(&data[DOMAINSIZE+2*(SUBDOMAIN)], SUBDOMAIN, MPI_DOUBLE, rank_bottom, 0, comm_cart, &requests[3]);

    // Left and Right
    MPI_Irecv(&data[0], 1, column_type, rank_left, 2, comm_cart, &requests[4]);
    MPI_Irecv(&data[DOMAINSIZE-1], 1, column_type, rank_right, 3, comm_cart, &requests[5]);
    MPI_Isend(&data[1], 1, column_type, rank_left, 3, comm_cart, &requests[6]);
    MPI_Isend(&data[DOMAINSIZE-2], 1, column_type, rank_right, 2, comm_cart, &requests[7]);

    MPI_Waitall(8, requests, statuses);

    // Handle corner cells
    int corner_ranks[4];
    MPI_Cart_shift(comm_cart, 0, 1, &corner_ranks[0], &corner_ranks[1]);  // top-left, bottom-left
    MPI_Cart_shift(comm_cart, 1, 1, &corner_ranks[2], &corner_ranks[3]);  // top-right, bottom-right

    double corner_values[4];
    MPI_Request corner_requests[8];
    MPI_Status corner_statuses[8];

    // Send corner values
    corner_values[0] = data[DOMAINSIZE+1];  // Top-left
    corner_values[1] = data[DOMAINSIZE+SUBDOMAIN];  // Top-right
    corner_values[2] = data[DOMAINSIZE*(SUBDOMAIN+1)+1];  // Bottom-left
    corner_values[3] = data[DOMAINSIZE*(SUBDOMAIN+1)+SUBDOMAIN];  // Bottom-right

    MPI_Isend(&corner_values[0], 1, MPI_DOUBLE, corner_ranks[0], 4, comm_cart, &corner_requests[0]);
    MPI_Isend(&corner_values[1], 1, MPI_DOUBLE, corner_ranks[3], 5, comm_cart, &corner_requests[1]);
    MPI_Isend(&corner_values[2], 1, MPI_DOUBLE, corner_ranks[1], 6, comm_cart, &corner_requests[2]);
    MPI_Isend(&corner_values[3], 1, MPI_DOUBLE, corner_ranks[3], 7, comm_cart, &corner_requests[3]);

    // Receive corner values
    MPI_Irecv(&data[0], 1, MPI_DOUBLE, corner_ranks[1], 4, comm_cart, &corner_requests[4]);
    MPI_Irecv(&data[DOMAINSIZE-1], 1, MPI_DOUBLE, corner_ranks[2], 5, comm_cart, &corner_requests[5]);
    MPI_Irecv(&data[DOMAINSIZE*(DOMAINSIZE-1)], 1, MPI_DOUBLE, corner_ranks[0], 6, comm_cart, &corner_requests[6]);
    MPI_Irecv(&data[DOMAINSIZE*DOMAINSIZE-1], 1, MPI_DOUBLE, corner_ranks[2], 7, comm_cart, &corner_requests[7]);

    MPI_Waitall(8, corner_requests, corner_statuses);

    if (rank == 9) {
        printf("data of rank 9 after communication\n");
        for (i = 0; i < DOMAINSIZE; i++) {
            for (j = 0; j < DOMAINSIZE; j++) {
                printf("%4.1f ", data[i*DOMAINSIZE + j]);
            }
            printf("\n");
        }
    }

    // Free MPI resources (e.g., types and communicators)
    // TODO

    MPI_Type_free(&column_type);
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();




    return 0;
}
