#include "filters.h"

int lowPass(int x, int x6, int x12, int y1, int y2){
	int fValue;

	fValue = 2*y1-y2+(x-2*x6+x12)/32;

	return fValue;
}

int highPass(int x, int x16, int x17, int x32, int y1){
	int fValue;

	fValue = y1-(x/32)+x16-x17+(x32/32);

	return fValue;
}

int derivative(int x, int x1, int x3, int x4){
	int fValue;

	fValue = (2*x+x1-x3-2*x4)/8;

	return fValue;
}

int squaring(int x){
	int fValue;

	fValue = x*x;

	return fValue;
}


int MWI(int xN[]){
	int N = 30;
	int sum = 0;
	int fValue;

	for(int i = 0; i < N;i++){
		sum += xN[i];
	}

	fValue = sum/N;
	return fValue;
}

