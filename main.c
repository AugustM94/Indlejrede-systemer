#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//This is a test comment to check if commits works properly... GG

int main(){
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename,"r"); //Reads file into variable file.
	int i = 0;
	int fileLength = 1000; //Number of data points in ECG.txt
	int value;
	int a[33] = {0}; //Initialize array with zeros only.

	int filteredLow[33] = {0};
	int lowY1 = 0;
	int lowY2 = 0;
	int fValueLow = 0;

	int fValueHigh = 0;
	int filteredHigh[33] = {0};

	int fValueDerivative = 0;
	int filteredDerivative[33] = {0};

	while(i < 100){
		value = getNextData(file);
		a[i%33] = value;

		//printf("data = %d : %d\n",value,-((i-6)>-1) & a[(i-6)%32]);
		// Low pass filter
		fValueLow = lowPass(a[i%33],
				-((i-6)>-1) & a[(i-6)%33],
				-((i-12)>-1) & a[(i-12)%33],
				lowY1,
				lowY2);
		//printf("i: %d value: %d\n",i, fValueLow);

		lowY2 = lowY1;
		lowY1 = fValueLow;

		filteredLow[i%33] = fValueLow;

		//High pass filter
		fValueHigh = highPass(
				filteredLow[i%33],
				-((i-16)>-1) & filteredLow[(i-16)%33],
				-((i-17)>-1) & filteredLow[(i-17)%33],
				-((i-32)>-1) & filteredLow[(i-32)%33],
				fValueHigh);

		filteredHigh[i%33] = fValueHigh;
		//printf("i: %d value: %d\n",i, fValueHigh);
		//printf("i: %d fValueHigh: %d %d\n",i%33,filteredLow[(i+1)%33], filteredLow[i%33]);

		// Derivative filter
		fValueDerivative = derivative(filteredHigh[i%33],
				-((i-1)>-1) & filteredHigh[(i-1)%33],
				-((i-3)>-1) & filteredHigh[(i-3)%33],
				-((i-4)>-1) & filteredHigh[(i-4)%33]
		);
		filteredDerivative[i%33] = fValueDerivative;

	    printf("i: %d value: fValueDerivative: %d \n",i,fValueDerivative);

		//usleep(4*1000); // This only works for unix systems.

		i++;
	}
	return 0;
}




