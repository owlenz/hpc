#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100000

enum operation {
	ENCODE,
	DECODE
};

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	int rank,world;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world);
	
	MPI_Status status;
	if(rank == 0)
	{
		int x;
		printf("1-encode\n2-decode\n");
		scanf("%d", &x);
		printf("provide the string\n");
		
		char *str = malloc(MAX_STR * sizeof(char));
		scanf("%s", str);
		
		int size = strlen(str);
		str = realloc(str, size);

		int segment_size = size /(world-1); 
		int change = size %(world-1); 
		enum operation op = x == 1 ? ENCODE : DECODE;
		for(int i = 1; i < world; i++)
		{
      int current_segment_size = segment_size + (i <= change ? 1 : 0);
			MPI_Send(&current_segment_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&op, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&str[(i-1) * segment_size + (i <= change ? i-1 : change)],
							current_segment_size,
							MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		char str_processed[size + 1];
		for(int i = 1; i < world; i++)
		{
      int current_segment_size = segment_size + (i <= change ? 1 : 0);
			MPI_Recv(&str_processed[(i-1) * segment_size + (i <= change ? i-1 : change)],
							current_segment_size,
							MPI_CHAR, i, 0, MPI_COMM_WORLD,&status);
		}
		str_processed[size] = '\0';
		printf("final string %s\n", str_processed);

	} else
	{
		int str_size;
		enum operation op;

		MPI_Recv(&str_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&op, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		char str[str_size];
		MPI_Recv(&str, str_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
		
		str[str_size] = '\0';

		if(op == ENCODE) 
		{
			for(int i = 0; i < str_size; i++)
			{
				str[i]+=3;
			}
		} else
		{
			for(int i = 0; i < str_size; i++)
			{
				str[i]-=3;
			}
		}
		printf("slave#%d outputs str %s\n", rank, str);

		MPI_Send(&str,str_size,MPI_CHAR,0,0,MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
