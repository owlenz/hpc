#include <limits.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ABORT_ERROR(code, msg) do \
{\
	fprintf(stderr, msg "\n");\
  MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE); \
} while(0)

#define ABORT_PERROR(code, msg) do \
{\
	perror(msg);\
  MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE); \
} while(0)


int main(int argc, char **argv) 
{
  MPI_Init(&argc, &argv);
	int xddmors = 10;
  int rank, world;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world);

	if(world == 1) 
	{
		ABORT_ERROR(1, "There is no slaves to utilize (only 1 processor available)");
	}

  if (rank == 0) 
	{
    int arr_size;
    printf("Please enter size of array...\n");
    scanf("%d", &arr_size);

    int *arr = malloc(arr_size * sizeof(int));
		if(arr == NULL)
			ABORT_PERROR(1, "memory allocation failed");

    printf("Please enter array elements...\n");
    for (int i = 0; i < arr_size; i++) 
		{
      scanf("%d", &arr[i]);
    };
  	int segment_size = arr_size / (world - 1);
		int change = arr_size % (world - 1);
		int balls = change;
		
		for(int i = 1; i < world; i++)
		{
      int current_segment_size = segment_size + (i <= change ? 1 : 0);
			MPI_Send(&current_segment_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&arr[(i-1) * segment_size + (i <= change ? i-1 : change)],
							current_segment_size,
							MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		int max_all = INT_MIN, max;
		int max_index_all = 0, max_index;
		MPI_Status status;
		for(int i = 1; i < world; i++)
		{
			MPI_Recv(&max, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&max_index, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			if(max > max_all)
			{
				max_all = max;
				max_index_all = max_index + ((i-1) * segment_size + (i <= change ? i-1 : change));
			}
		}
		printf("max at master process is %d with index %d \n",max_all,max_index_all);
		free(arr);
  } else
	{
		MPI_Status status;
		int seg_size;
		MPI_Recv(&seg_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		int max = INT_MIN;
		int max_index = 0;
		int arr[seg_size];
		MPI_Recv(arr, seg_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		for (int i = 0; i < seg_size;i++) 
		{
			if (max < arr[i]){
				max = arr[i];
				max_index= i;
			}
		}
		printf("max of slave#%d is %d and local index %d\n", rank, max, max_index);
		MPI_Send(&max,1,MPI_INT,0,0,MPI_COMM_WORLD);
		MPI_Send(&max_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	} 
	MPI_Finalize();
	
}
