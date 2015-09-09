#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"

int getNextData(FILE *file){
	// Implement me according to the Assignment 1 manual

	int value;

	fscanf(file,"%i",&value);

	return value;
}
