#include "funcHelp.h"


int average(int rrIntervals[]){
	int avg = 0;
	int sum = 0;
	int aboveZero = 0;
	for(int i = 0; i < sizeof(rrIntervals); i++){
		if (rrIntervals[i] > 0){
			//do something faster with bit shifting to avoid if statement
			aboveZero++;
		}
		sum += rrIntervals[i];
	}
	avg = sum/aboveZero;
	return avg;
}

