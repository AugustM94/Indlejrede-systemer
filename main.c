#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename,"r"); //Reads file into variable file.
	int i = 0;
	int fileLength = 1000; //Number of data points in ECG.txt
	int value;
	int a[32] = {0}; //Initialize array with zeros only.

	int filteredLow[32] = {0};
	int lowY1 = 0;
	int lowY2 = 0;
	int fValueLow = 0;

	int fValueHigh = 0;
	int filteredHigh[32] = {0};

	int fValueDerivative = 0;
	int filteredDerivative[32] = {0};

	while(i < 100){
		value = getNextData(file);
		a[i%32] = value;

		//printf("data = %d : %d\n",value,-((i-6)>-1) & a[(i-6)%32]);
		// Low pass filter
		fValueLow = lowPass(a[i%32],
				-((i-6)>-1) & a[(i-6)%32],
				-((i-12)>-1) & a[(i-12)%32],
				lowY1,
				lowY2);
		//printf("%d\n",fValueLow);

		lowY2 = lowY1;
		lowY1 = fValueLow;

		filteredLow[i%32] = fValueLow;

		//High pass filter
		fValueHigh = highPass(
				filteredLow[i%32],
				-((i-16)>-1) & filteredLow[(i-16)%32],
				-((i-17)>-1) & filteredLow[(i-17)%32],
				-((i-32)>-1) & filteredLow[(i-32)%32],
				fValueHigh);

		filteredHigh[i%32] = fValueHigh;
		//printf("fValueHigh: %d\n",fValueHigh);

		// Derivative filter
		fValueDerivative = derivative(filteredHigh[i%32],
				-((i-1)>-1) & filteredHigh[(i-1)%32],
				-((i-3)>-1) & filteredHigh[(i-3)%32],
				-((i-4)>-1) & filteredHigh[(i-4)%32]
		);
		filteredDerivative[i%32] = fValueDerivative;

		printf("fValueDerivative: %d\n",fValueDerivative);

		//usleep(4*1000); // This only works for unix systems.

		i++;
	}
	return 0;
}




