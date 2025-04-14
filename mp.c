#include <omp.h>
#include <stdio.h>
#include <time.h>

int main() {
	long n = 1000000000;
  double step = 1.0 / n;
  double sum = 0.0;
	double x = 0;
	clock_t current_c = clock();
#pragma omp parallel for private(x) reduction(+:sum)
  for (int i = 0; i < n; i++) {
    x = (i + 0.5) * step;
    sum = sum +  4.0 / (1.0 + x * x);
  }
	clock_t after_c = clock();
	double elapsed = (double)(after_c-current_c)/CLOCKS_PER_SEC;
	printf("time %f\n",elapsed);
  double pi = sum * step;
  printf("balls %.40f\n", pi);
}
