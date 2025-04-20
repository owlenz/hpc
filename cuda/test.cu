#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARR_SIZE 100000000
#define BLOCK_SIZE 256
__global__ void balls(int a[], int b[], int c[]) { int i = blockIdx.x * blockDim.x * threadIdx.x; c[i] = a[i] + b[i]; }


int main(int argc, char **argv)
{
	srand(time(NULL));
	int *a = (int *)malloc(ARR_SIZE * sizeof(int));
	int *b = (int *)malloc(ARR_SIZE * sizeof(int));
	int *c = (int *)malloc(ARR_SIZE * sizeof(int));
	for(int i = 0;i < ARR_SIZE; i++)
	{
		a[i] = rand() /  10000;
		b[i] = rand() /  10000;
	}
	int *dev_a;
	int *dev_b;
	int *dev_c;
	
	cudaMalloc((void **)&dev_a, sizeof(int) * ARR_SIZE);
	cudaMalloc((void **)&dev_b, sizeof(int) * ARR_SIZE);
	cudaMalloc((void **)&dev_c, sizeof(int) * ARR_SIZE);

	cudaMemcpy(dev_a, a, sizeof(int) * ARR_SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, b, sizeof(int) * ARR_SIZE, cudaMemcpyHostToDevice);
	int numBlocks = (ARR_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
	balls<<<numBlocks, BLOCK_SIZE>>>(dev_a, dev_b, dev_c);
	cudaMemcpy(c, dev_c, sizeof(int) * ARR_SIZE , cudaMemcpyDeviceToHost);
	// printf("%d + %d = %d\n", a[0], b[0], c[0]);
	// printf("%d + %d = %d\n", a[1], b[1], c[1]);
	// printf("%d + %d = %d\n", a[2], b[2], c[2]);
	// printf("%d + %d = %d\n", a[3], b[3], c[3]);
	cudaFree(dev_a);
	cudaFree(dev_b);
	cudaFree(dev_c);
}
