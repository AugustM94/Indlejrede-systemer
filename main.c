#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename,"r"); //Reads file into variable file.
	int i = 0;
	int fileLength = 1000; //Number of data points in ECG.txt
	int a[32] = {0}; //Initialize array with zeros only.
	int lowY1 = 0;
	int lowY2 = 0;
	int fValue = 0;
	int value;

	while(i < 100){
		value = getNextData(file);
		a[i%32] = value;
		// Low pass filter

		fValue = lowPass(a[i%32],-((i-6)>0) & a[(i-6)%32],-((i-12)>0) & a[(i-12)%32],lowY1,lowY2);
		printf("%d\n",fValue);

		lowY2 = lowY1;
		lowY1 = fValue;

		//High pass filter



		//printf("i = %d : ecg = %d : y1 = %d \n",i,a[i%32],y1);

		//usleep(4*1000); // This only works for unix systems.

		i++;

	}


	return 0;
}




