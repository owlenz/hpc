#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARR_SIZE 100000000
void balls(int a[], int b[], int c[]) 
{
	for(int i = 0; i < ARR_SIZE; i++)
	{
		c[i] = a[i] + b[i];
	}
}


int main(int argc, char **argv)
{
	srand(time(NULL));
	int *a = (int *)malloc(ARR_SIZE * sizeof(int));
	int *b = (int *)malloc(ARR_SIZE * sizeof(int));
	int *c = (int *)malloc(ARR_SIZE * sizeof(int));

	if (a == NULL || b == NULL || c == NULL) {
			fprintf(stderr, "memory allocation failed\n");
			return 1;
	}

	for(int i = 0;i < ARR_SIZE; i++)
	{
		a[i] = (float)rand() * 100000;
		b[i] = (float)rand() * 100000;
	}
	
	balls(a, b, c);
}
