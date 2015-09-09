#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename,"r"); //Reads file into variable file.
	int i = 1;
	int fileLength = 10000; //Number of data points in ECG.txt

	while(i < 1000){
		printf("%d\n",getNextData(file));
		usleep(4*1000); // This only works for unix systems.
		i++;
	}


	return 0;
}




