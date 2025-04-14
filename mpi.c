#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void random_arr(uint8_t *arr, int size) 
{
  srand(time(NULL));
  for (int i = 0; i < size; i++) 
	{
    arr[i] = ((float)rand() / RAND_MAX) * 50;
  }
}

int main(int argc, char **argv) 
{
  uint8_t arr[20];
  MPI_Init(&argc, &argv);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int segment_size = 20 / world_size;
  // if array size not divisible by the number of proc
  int change = 20 % world_size;
  
	double start = MPI_Wtime();
	if (rank == 0) {
    srand(time(NULL));
    random_arr(arr, 20);
    // logging
    int sum = 0;
    for (int i = 0; i < 20; i++) {
      printf("Generated value: %d\n", arr[i]);
      sum += arr[i];
    }
    printf("sum to get %d\n", sum);
  }

  MPI_Status status;
  if (rank != 0) {
    uint8_t segment[segment_size];

    int sum = 0;
    // for (int i = 0; i < segment_size; i++) {
    //   MPI_Recv(&segment[i], 1, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD,
    //            &status);
    //   sum += segment[i];
    //   printf("message %d recieved: %d: \n", world_rank, segment[i]);
    // }

    MPI_Recv(segment, segment_size, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD,
               &status);
    for (int i = 0; i < segment_size; i++) {
      sum += segment[i];
    }
    printf("sum to send %d: \n", sum);
    MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    /* printf("sum after %d:from %d\n", sum,rank);
    printf("sum after %d:from %d\n", sum,rank);
    printf("sum after %d:from %d\n", sum,rank);
    printf("sum after %d:from %d\n", sum,rank);
    printf("sum after %d:from %d\n", sum,rank);
    printf("sum after %d:from %d\n", sum,rank);
    printf("sum after %d:from %d\n", sum,rank);
    printf("sum after %d:from %d\n", sum,rank); */
  } else {
    for (int i = 1; i < world_size; i++) {
      // for (int j = change; j < segment_size + change; j++) {
        MPI_Send(&arr[change + (segment_size * i)], segment_size,
                 MPI_UINT8_T, i, 0, MPI_COMM_WORLD);
      // }
    }
    int sum = 0;
    // for (int j = 0; j < segment_size + change; j++) {
    //   sum += arr[j];
    // }
    int message = 0;
    for (int i = 1; i < world_size; i++) {
      MPI_Recv(&message,1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      sum += message;
    }
    printf("sum of all %d\n", sum);
  }

  MPI_Finalize();

	double end = MPI_Wtime();
	printf("time %f\n",end-start);
  return 0;
}
