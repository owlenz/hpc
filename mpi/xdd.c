#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
	srand(time(NULL));
	int i = 800;
	while(i--){
		printf("%d ", (int)(((float)rand()/RAND_MAX) * 100000));
	}
}
