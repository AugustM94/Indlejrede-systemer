#include "funcHelp.h"

int average(int rrIntervals[]){
	// Take an array of int's and return the average of the values.
	int sum = 0;
	int aboveZero = 0;

	for(int i = 0; i < sizeof(rrIntervals); i++){
		if (rrIntervals[i] != 0){
			aboveZero++;
		}
		sum += rrIntervals[i];
	}

	return sum/aboveZero;
}

