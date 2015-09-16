#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename,"r"); //Reads file into variable file.
	int i = 0;
	int fileLength = 10000; //Number of data points in ECG.txt
	int a[32];
	int value;



	while(i < 32){
		value = getNextData(file);
		a[i] = value;
		printf("Pikoghor %d\n",value);
		usleep(4*1000); // This only works for unix systems.
		i++;

	}

	/*
	for(int i = 0; i< 32; i++){
		printf("%d\n",a[i]);
	}
	*/

	return 0;
}




