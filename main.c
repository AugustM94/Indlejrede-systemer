/*
 * This program has been written as part of assignment 1 in course 02131
 * Authors: 	August Møbius & Martin Meincke
 * Date:		29. September 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	static const char filename[] = "ECG900K.txt";
	FILE *file = fopen(filename, "r"); // Reads file into variable file.
	int i = 0;
	int fileLength = 100000; // Number of data points in datafile

	// Filter variables
	int value;
	int a[33] = { 0 }; // Initialise array with zeros only.

	int filteredLow[33] = { 0 };
	int lowY1 = 0;
	int lowY2 = 0;
	int fValueLow = 0;

	int fValueHigh = 0;
	int filteredHigh[33] = { 0 };

	int fValueDerivative = 0;

	int valueSquared = 0;
	int valuesSqaured[30] = { 0 };

	int MWIValue = 0;

	// Peak variables
	int tempFindPeak[3] = { 0 }; // Saves the 3 most recent values that have passed all filters.
	int newPeakIndex; // Index of recent peak.
	int peaks[5000];	// Consider using a storage type with dynamic size.
	int peakIndex[5000];	// Consider using a storage type with dynamic size.
	int rPeaks[2000]; // Consider using a storage type with dynamic size.
	int rPeakIndex[2000];	// Consider using a storage type with dynamic size.
	int rPeaksPointer = 0;
	int peaksPointer = 0;
	int THRESHOLD1 = 1875;
	int THRESHOLD2 = 938;
	int NPKF = 1000;
	int SPKF = 4500;

	// RR average variables
	int RR_Average1 = 0;
	int RR_Average2 = 0;
	int RR = 0;
	int RecentRR[8] = { 0 };
	int RecentRRPointer = 0;
	int RecentRR_OK[8] = { 0 };
	int RecentRROKPointer = 0;

	// RR High, low and miss
	int RR_CURRENT = 0; //Used for interval calculation.
	int RR_LAST = 0;	//Used for interval calculation.
	int RR_LOW = 138;
	int RR_HIGH = 174;
	int RR_MISS = 249;

	// Counter for successive RR misses
	int missCounter = 0;

	while (i < fileLength) {
		value = getNextData(file);
		a[i % 33] = value;

		// Low pass filter
		fValueLow = lowPass(a[i % 33], -((i - 6) > -1) & a[(i - 6) % 33],
				-((i - 12) > -1) & a[(i - 12) % 33], lowY1, lowY2);

		lowY2 = lowY1;
		lowY1 = fValueLow;

		filteredLow[i % 33] = fValueLow;

		// High pass filter
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

		// Squared
		valueSquared = squaring(fValueDerivative);
		valuesSqaured[i % 30] = valueSquared;

		// MWI filter
		MWIValue = MWI(valuesSqaured);
		tempFindPeak[i % 3] = MWIValue;

		// Find peaks as a local maxima using bit-shifting
		if ((-((i - 1) > -1) & tempFindPeak[(i - 1) % 3])
				> (-((i - 2) > -1) & tempFindPeak[(i - 2) % 3])
				&& (-((i - 1) > -1) & tempFindPeak[(i - 1) % 3])
						> tempFindPeak[i % 3]) {
			int newPeak = tempFindPeak[(i - 1) % 3];
			newPeakIndex = i - 1;

			// Adds peak to array of peaks.
			peaks[peaksPointer] = newPeak;
			peakIndex[peaksPointer] = i-1;
			peaksPointer++;

			// Check if peak is below threshold 1
			if (newPeak <= THRESHOLD1) {
				// Update NPKF and thresholds
				NPKF = newPeak/8 + (7*NPKF)/8;
				THRESHOLD1 = NPKF +(SPKF - NPKF)/4;
				THRESHOLD2 = THRESHOLD1 / 2;
			} else {

				// Calculate the pulse as beats per minute.
				int BPM = 0;
				for(int j = rPeaksPointer-1; j > 0; j--){
					if(rPeakIndex[j]+15000 > newPeakIndex){
						BPM++;
					}
				}

				// Check if R-peak is below 2000
				if(newPeak < 2000){
					printf("The R-peak value is less than 2000.\n");
				}

				// Calculate RR value
				RR_CURRENT = i-1;
				RR = RR_CURRENT - RR_LAST;

				// Print the pulse, r-peak value, the time since start, and time since last peak
				printf("Pulse is %d BPM.\nCurrent R-peak value: %d. The peak occurred %d sec after start. It has been %d ms since last R-peak. \n",BPM,newPeak, newPeakIndex/250, (RR*1000)/250);

				if(RR_LOW < RR && RR < RR_HIGH){
					// Store peak in rPeaks
					rPeaks[rPeaksPointer] = newPeak;
					rPeakIndex[rPeaksPointer] = newPeakIndex;
					rPeaksPointer++;

					RR_LAST = newPeakIndex;

					// Update SPKF
					SPKF = newPeak/8 + (7*SPKF)/8;

					// Update recent RR arrays.
					RecentRR[RecentRRPointer % 8] = RR;
					RecentRR_OK[RecentRROKPointer % 8] = RR;

					RR_Average1 = average(RecentRR);
					RR_Average2 = average(RecentRR_OK);

					RecentRROKPointer++;
					RecentRRPointer++;
					// Update RR_HIGH, RR_LOW and RR_MISS

					RR_LOW = 0.92 * RR_Average2;
					RR_HIGH = 1.16 * RR_Average2;
					RR_MISS = 1.66 * RR_Average2;

					// Update thresholds
					THRESHOLD1 = NPKF + (SPKF-NPKF)/4;
					THRESHOLD2 = THRESHOLD1/2;

					// Reset missCounter
					missCounter = 0;

				} else {
					missCounter++;
					if(missCounter >= 5){
						printf("Missed %d successive RR intervals.\n",missCounter);
					}
					if(RR > RR_MISS){
						// Initialise searchback
						for(int j = peaksPointer-1; j > -1; j--){
							if(peaks[j] > THRESHOLD2){
								int newRPeak = peaks[j];
								// Store peak in R peak
								rPeaks[rPeaksPointer]= newRPeak;
								rPeakIndex[rPeaksPointer] = peakIndex[j];
								rPeaksPointer++;

								// SPKF update
								SPKF = newRPeak/4 + (3*SPKF)/4;

								// Store RR in RecentRR
								RecentRR[RecentRRPointer % 8] = RR;
								RR_LAST = newPeakIndex;

								// Update average 1
								RR_Average1 = average(RecentRR);

								// RR updates
								RR_LOW = 0.92 * RR_Average1;
								RR_HIGH = 1.16 * RR_Average1;
								RR_MISS = 1.66 * RR_Average1;

								//Update THRESHOLDS
								THRESHOLD1 = NPKF + (SPKF-NPKF)/4;
								THRESHOLD2 = THRESHOLD1/2;
							}
							break;
						}
					}
				}
			}
			printf("\n");
		}
		i++;
	}

	return 0;
}

