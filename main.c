#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//This is a test comment to check if commits works properly... GG

int main() {
	static const char filename[] = "ECG.txt";
	FILE *file = fopen(filename, "r"); //Reads file into variable file.
	int i = 0;
	int fileLength = 10000; //Number of data points in ECG.txt

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
	int newPeakIndex; //Index of recent peak.
	int peaks[5000];
	int peakIndex[5000];
	int rPeaks[2000];
	int rPeaksPointer = 0;
	int peaksPointer = 0;
	int THRESHOLD1 = 1875;
	int THRESHOLD2 = 938;
	int NPKF = 625;
	int SPKF = 5000;

	//RR average variables
	int RR_Average1 = 0;
	int RR_Average2 = 0;
	int RR = 0;
	int RecentRR[8] = { 0 };
	int RecentRRPointer = 0;
	int RecentRR_OK[8] = { 0 };
	int RecentRROKPointer = 0;

	//RR High, low and miss
	int RR_CURRENT = 0; //Used for interval calculation.
	int RR_LAST = 0;	//Used for interval calculation.
	int RR_LOW = 138;
	int RR_HIGH = 174;
	int RR_MISS = 249;


	while (i < 10000) {
		value = getNextData(file);
		a[i % 33] = value;

		// Low pass filter
		fValueLow = lowPass(a[i % 33], -((i - 6) > -1) & a[(i - 6) % 33],
				-((i - 12) > -1) & a[(i - 12) % 33], lowY1, lowY2);

		lowY2 = lowY1;
		lowY1 = fValueLow;

		filteredLow[i % 33] = fValueLow;

		//High pass filter
		fValueHigh = highPass(filteredLow[i % 33],
				-((i - 16) > -1) & filteredLow[(i - 16) % 33],
				-((i - 17) > -1) & filteredLow[(i - 17) % 33],
				-((i - 32) > -1) & filteredLow[(i - 32) % 33], fValueHigh);

		filteredHigh[i % 33] = fValueHigh;

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
			int newPeak = tempFindPeak[(i - 1) % 3];
			newPeakIndex = i - 1;

			//Adds peak to array of peaks.
			peaks[peaksPointer] = newPeak;
			peakIndex[peaksPointer] = i-1;
			peaksPointer++;
			printf("\nPeak: %d index: %d \n",peaks[peaksPointer-1],i);
			printf("\nThreshold1: %d\n",THRESHOLD1);
			// Peak > THRESHOLD1?
			if (newPeak <= THRESHOLD1) { // is peak below the threshold
				printf("CASE 1\n");
				NPKF = newPeak/8 + (7*NPKF)/8;
				THRESHOLD1 = NPKF +(SPKF - NPKF)/4;
				THRESHOLD2 = THRESHOLD1 / 2;
			} else { // is peak above the threshold
				//Calculate RR value
				//RR = i - (-((rrAverage2Pointer) > -1) & RecentRR_OK[(rrAverage2Pointer) % 8]);

				RR_CURRENT = i-1;
				printf("RR_CURRENT = %d, RR_LAST = %d\n",RR_CURRENT, RR_LAST);
				RR = RR_CURRENT - RR_LAST;
				//RR_LAST = RR_CURRENT;

				printf("Calculating new RR = %d for i = %d\n ", RR, i);
				if(RR_LOW < RR && RR < RR_HIGH){
					printf("CASE 2\n");
					printf("New RR Peak found: RR_LOW = %d, RR = %d, RR_HIGH = %d\n",RR_LOW, RR, RR_HIGH);
					//Store peak in rPeaks
					rPeaks[rPeaksPointer] = newPeak;
					rPeaksPointer++;
					RR_LAST = newPeakIndex;

					// update SPKF
					SPKF = newPeak/8 + (7*SPKF)/8;
					//Store RR in recent RR

					//update recent RR arrays.
					RecentRR[RecentRRPointer % 8] = RR;
					RecentRR_OK[RecentRROKPointer % 8] = RR;

					RR_Average1 = average(RecentRR);
					RR_Average2 = average(RecentRR_OK);

					RecentRROKPointer++;
					RecentRRPointer++;
					// update RR_HIGH, RR_LOW and RR_MISS

					RR_LOW = 0.92 * RR_Average2;
					RR_HIGH = 1.16 * RR_Average2;
					RR_MISS = 1.66 * RR_Average2;

					// update thresholds
					printf("NPKF = %d\n",NPKF);
					THRESHOLD1 = NPKF + (SPKF-NPKF)/4;
					THRESHOLD2 = THRESHOLD1/2;

				} else {
					if(RR > RR_MISS){
						printf("CASE 4\n");
						printf("RR = %d > RR_MISS = %d\n", RR, RR_MISS);
						//Initialize searchback
						for(int j = peaksPointer-1; j > -1; j--){
							if(peaks[j] > THRESHOLD2){
								int newRPeak = peaks[j];
								//Store peak in R peak
								rPeaks[rPeaksPointer]= newRPeak;
								rPeaksPointer++;

								//SPKF update
								SPKF = newRPeak/4 + (3*SPKF)/4;

								//Store RR in RecentRR
								RecentRR[RecentRRPointer % 8] = RR;

								//Update average 1
								RR_Average1 = average(RecentRR);

								//RR updates
								RR_LOW = 0.92 * RR_Average1;
								RR_HIGH = 1.16 * RR_Average1;
								RR_MISS = 1.66 * RR_Average1;

								//Update THRESHOLDS
								THRESHOLD1 = NPKF + (SPKF-NPKF)/4;
								THRESHOLD2 = THRESHOLD1/2;


							}
						}
					}else{
						printf("CASE 3\n");
					}

				}
			}
		}


		//usleep(4*1000); // This only works for unix systems.
		i++;
	}
	for(int i = 0; i < 17; i++){
		printf("%d\n",rPeaks[i]);
	}

	return 0;
}

