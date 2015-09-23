#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//This is a test comment to check if commits works properly... GG

int main() {
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename, "r"); //Reads file into variable file.
	int i = 0;
	int fileLength = 1000; //Number of data points in ECG.txt

	//Filter variables
	int value;
	int a[33] = { 0 }; //Initialize array with zeros only.

	int filteredLow[33] = { 0 };
	int lowY1 = 0;
	int lowY2 = 0;
	int fValueLow = 0;

	int fValueHigh = 0;
	int filteredHigh[33] = { 0 };

	int fValueDerivative = 0;
	int filteredDerivative[33] = { 0 };

	int valueSquared = 0;
	int valuesSqaured[30] = { 0 };

	int MWIValue = 0;

	//Peak detection variables
	int tempFindPeak[3] = { 0 }; //Saves the 3 most recent values that have passed all filters.
	int peaks[5000];
	int rPeaks[8];
	int peaksPointer = 0;
	int THRESHOLD1 = 0;
	int THRESHOLD2 = 0;
	int NPKF = 0;
	int SPKF = 0;

	//RR average variables
	int RR = 0;
	int rrAverage2[8] = { 0 };
	int rrAverage2Pointer = 0;

	while (i < 10000) {
		value = getNextData(file);
		a[i % 33] = value;

		//printf("data = %d : %d\n",value,-((i-6)>-1) & a[(i-6)%32]);
		// Low pass filter
		fValueLow = lowPass(a[i % 33], -((i - 6) > -1) & a[(i - 6) % 33],
				-((i - 12) > -1) & a[(i - 12) % 33], lowY1, lowY2);
		//printf("i: %d value: %d\n",i, fValueLow);

		lowY2 = lowY1;
		lowY1 = fValueLow;

		filteredLow[i % 33] = fValueLow;

		//High pass filter
		fValueHigh = highPass(filteredLow[i % 33],
				-((i - 16) > -1) & filteredLow[(i - 16) % 33],
				-((i - 17) > -1) & filteredLow[(i - 17) % 33],
				-((i - 32) > -1) & filteredLow[(i - 32) % 33], fValueHigh);

		filteredHigh[i % 33] = fValueHigh;
		//printf("i: %d value: %d\n",i, fValueHigh);
		//printf("i: %d fValueHigh: %d %d\n",i%33,filteredLow[(i+1)%33], filteredLow[i%33]);

		// Derivative filter
		fValueDerivative = derivative(filteredHigh[i % 33],
				-((i - 1) > -1) & filteredHigh[(i - 1) % 33],
				-((i - 3) > -1) & filteredHigh[(i - 3) % 33],
				-((i - 4) > -1) & filteredHigh[(i - 4) % 33]);
		filteredDerivative[i % 33] = fValueDerivative;

		//printf("i: %d value: fValueDerivative: %d \n",i,fValueDerivative);

		//Squared
		valueSquared = squaring(fValueDerivative);
		valuesSqaured[i % 30] = valueSquared;

		//MWI filter
		MWIValue = MWI(valuesSqaured);
		//printf("%d\n",MWIValue);

		tempFindPeak[i % 3] = MWIValue;

		//Find peaks
		if ((-((i - 1) > -1) & tempFindPeak[(i - 1) % 3])
				> (-((i - 2) > -1) & tempFindPeak[(i - 2) % 3])
				&& (-((i - 1) > -1) & tempFindPeak[(i - 1) % 3])
						> tempFindPeak[i % 3]) {
			//printf("i = %d : %d : %d : %d \n",i,tempFindPeak[(i-2)%3],tempFindPeak[(i-1)%3],tempFindPeak[i%3]);
			int newPeak = tempFindPeak[(i - 1) % 3];

			//Adds peak to array of peaks.
			peaks[peaksPointer] = newPeak;
			peaksPointer++;

			// Peak > THRESHOLD1?
			if (newPeak < THRESHOLD1) { // No!
				NPKF = 0.125 * newPeak + 0.875 * NPKF;
				THRESHOLD1 = NPKF + 0.25 * (SPKF - NPKF);
				THRESHOLD2 = THRESHOLD1 / 2;
			} else { // Yes! - Save RR interval
				rrAverage2[rrAverage2Pointer % 8] = i;
				//Calculate RR value
				RR = rrAverage2[rrAverage2Pointer % 8] - ((-(rrAverage2Pointer - 1) > -1) & rrAverage2[(rrAverage2Pointer - 1) % 8]);
				printf("RA %d : RA-1 %d : RR = %d\n",rrAverage2[rrAverage2Pointer % 8],(-(rrAverage2Pointer - 1) > -1) & rrAverage2[(rrAverage2Pointer - 1) % 8],RR);


				rrAverage2Pointer++;
			}

		}

		//

		//usleep(4*1000); // This only works for unix systems.

		i++;
	}

	for (int i = 0; i < 200; i++) {
	//	printf("%d\n", peaks[i]);
	}
	return 0;
}

