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
	int peakIndex[5000];
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
	int rrAverage1[8] = { 0 };
	int rrAverage1Pointer = 0;

	//RR High, low and miss
	int RR_LOW = 0;
	int RR_HIGH = 1000;
	int RR_MISS = 0;

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
		//printf("i: %d %d\n",i, MWIValue);

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
			peakIndex[peaksPointer] = i;
			peaksPointer++;

			printf("\n");
			//printf("  THRESHOLD 1 : %d \n", THRESHOLD1);
			// Peak > THRESHOLD1?
			if (newPeak <= THRESHOLD1) { // is peak below the threshold
				printf("peak is BELOW threshold \n");
				NPKF = newPeak/8 + NPKF/8;
				THRESHOLD1 = NPKF +(SPKF - NPKF)/4;
				THRESHOLD2 = THRESHOLD1 / 2;
			} else { // is peak above the threshold
				printf("peak is ABOVE threshold \n");
				//Calculate RR value
				RR = i - (-((rrAverage2Pointer - 1) > -1) & rrAverage2[(rrAverage2Pointer - 1) % 8]);

				//printf("i %d :RA %d : RA-1 %d : RR = %d\n",i, rrAverage2[rrAverage2Pointer % 8],-((rrAverage2Pointer - 1) > -1) & rrAverage2[(rrAverage2Pointer - 1) % 8],RR);
				//printf("low: %d, high: %d, miss: %d\n",RR_LOW,RR_HIGH,RR_MISS);
				//printf("%d , %d , %d\n ",RR_LOW, RR, RR_HIGH);

				if(RR_LOW < RR && RR < RR_HIGH){
					printf("RR is between high and low %d\n",i);
					//printf("i: %d RR: %d\n",i,RR);
					//update recent RR arrays.
					rrAverage2[rrAverage2Pointer % 8] = i;
					rrAverage1[rrAverage1Pointer % 8] = i;
					// update SPKF
					SPKF = newPeak/8 + 7*SPKF/8;
					//Store RR in recent RR

					// update RR_HIGH, RR_LOW and RR_MISS
					int RR_average2 = average(rrAverage2);
					RR_LOW = 0.92 * RR_average2;
					RR_HIGH = 1.16 * RR_average2;
					RR_MISS = 1.66 * RR_average2;
					//printf("low: %d, high: %d, miss: %d\n",RR_LOW,RR_HIGH,RR_MISS);

					// update thresholds
					THRESHOLD1 = NPKF + (SPKF-NPKF)/4;
					THRESHOLD2 = THRESHOLD1/2;

					rrAverage2Pointer++;
					rrAverage1Pointer++;
				} else {
					printf("peak is NOT between high and low : %d\n",i);

					if(RR > RR_MISS){
						printf("RR is ABOVE RR_miss\n");
						//printf("number of peaks: %d \n ", peaksPointer);

						for(int i = peaksPointer; i > 0; i--){
							if(peaks[i] > THRESHOLD2){
								printf("found SEARCHBACK peak: %d\n", peakIndex[i]);
								//printf("i: %d RR: %d\n",i,RR);
								rrAverage1[rrAverage1Pointer % 8] = peakIndex[i];
								SPKF = peaks[i]/4 + (3* SPKF)/4;

								int RR_average1 = average(rrAverage1);

								//update RR_LOW, RR_HIGH and RR_MISS
								RR_LOW = 0.92 * RR_average1;
								RR_HIGH = 1.16 * RR_average1;
								RR_MISS = 1.66 * RR_average1;

								// update thresholds
								THRESHOLD1 = NPKF + (SPKF - NPKF)/4;
								THRESHOLD2 = THRESHOLD1/2;

								rrAverage1Pointer++;
								break;
							}
						}
					}
				}
			}
		}

		//usleep(4*1000); // This only works for unix systems.

		i++;
	}
/*
	for (int i = 0; i < 200; i++) {
	//	printf("%d\n", peaks[i]);
	}*/
	return 0;
}

