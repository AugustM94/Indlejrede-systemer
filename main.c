#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename,"r"); //Reads file into variable file.
	int i = 0;
	int fileLength = 1000; //Number of data points in ECG.txt
	int a[32];
	int b[32];
	int y1 = 0;
	int y2 = 0;
	int value;



	while(i < 1){
		value = getNextData(file);
		a[i] = value;
		printf("%d",y1);
		printf("%d",lowPass(a[0],0,0,y1,y2));
		//printf("Pikoghor %d\n",value);
		usleep(4*1000); // This only works for unix systems.
		i++;

	}

	//lowPass(a[32],a[26],a[20],a[31],a[30]);
	//printf("%d",lowPass(a[0],0,0,0,0));




	return 0;
}




