#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX_STR 100000

enum operation {
	ENCODE,
	DECODE
};

char encode(char x) {
	if (x > 64 && x <= 90){
		x+=3;
		if(x > 90){
			x = 64 + (x-90);
		}
	} else if (x > 96 && x <= 122)
	{
		x+=3;
		if(x > 122){
			x = 96 + (x-122);
		}
	}
	return x;
}

int decode(char x) {
	if (x >= 65 && x < 91){
		x-=3;
		if(x < 65){
			x = 91 - (65 - x);
		}
	} else if (x >= 97 && x < 123)
	{
		x-=3;
		if(x < 97){
			x = 123 - (97 - x);
		}
	}
	return x;
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	int rank,world;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world);
	
	if(world == 1) 
	{
		printf("There is no slaves to utilize (only 1 processor available)\n");
		MPI_Finalize();
		return 1;
	}


	MPI_Status status;
	if(rank == 0)
	{
		int x = 2;
		// printf("1-encode\n2-decode\n");
		// scanf("%d", &x);
		// getchar();
		// enum operation op = (x == 1) ? ENCODE : DECODE;
		enum operation op = ENCODE;
		
		// printf("1-input string\n2-read from text file\n");
		// scanf("%d", &x);
		// getchar();
		
		char *str = malloc(MAX_STR * sizeof(char));
		// read from text file
		if(x == 2)
		{
			int fd = open("input.txt", O_RDONLY, S_IRWXU);
			read(fd, str, MAX_STR);
		} else 
		{
			// read from user input
			printf("provide the string\n");
			if(fgets(str, MAX_STR, stdin) != NULL)
			{
				int len = strlen(str);
				str[len-1] = '\0';
			}
		} 
		
		int size = strlen(str);
		str[size-1] = '\0';
		str = realloc(str, size);

		int segment_size = size / (world-1); 
		int change = size % (world-1); 
		int offset = 0;

		// Send Substrings to Slaves
		for(int i = 1; i < world; i++)
		{
      int current_segment_size = segment_size + (i <= change ? 1 : 0);
			MPI_Send(&current_segment_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&op, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&str[offset],
							current_segment_size,
							MPI_CHAR, i, 0, MPI_COMM_WORLD);
			offset += current_segment_size;
		}

		offset = 0;
		char str_processed[size + 1];
		// Recieve Encrypted/Decrypted Substrings
		for(int i = 1; i < world; i++)
		{
      int current_segment_size = segment_size + (i <= change ? 1 : 0);
			MPI_Recv(&str_processed[offset],
							current_segment_size,
							MPI_CHAR, i, 0, MPI_COMM_WORLD,&status);

			offset += current_segment_size;
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

		printf("recv string: %s\n", str);
		if(op == ENCODE) 
		{
			for(int i = 0; i < str_size; i++)
			{
				str[i] = encode(str[i]);
			}
		} else
		{
			for(int i = 0; i < str_size; i++)
			{
				str[i] = decode(str[i]);
			}
		}
		// printf("slave#%d outputs str %s\n", rank, str);

		MPI_Send(&str,str_size,MPI_CHAR,0,0,MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
