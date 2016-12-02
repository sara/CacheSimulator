#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "c-sim.h"
#include <math.h>
int main(int argc, char** argv)
{
	//command line params to make the cache and policies
	int cacheSize = atoi(argv[1]);
	char* associativity = argv[2];
	int blockSize = atoi(argv[3]);
	char* replacementPolicy = argv[4];
	char* writePolicy = argv[5];
	FILE* inputFile = fopen(argv[6], "r");

	//data tracking to return
	int memReads = 0;
	int memWrites = 0;
	int hits = 0;
	int misses = 0;

	int association = -1;
	int numLines;
	int numSets;
	int* setIndexBits = (int*)malloc(sizeof(int));
	*setIndexBits = 0;
	int* blockOffsetBits = (int*)malloc(sizeof(int));
	*blockOffsetBits = 0;;
	int setIndexBitCalculator;
	numLines = cacheSize/blockSize;
	if (strcmp(associativity, "direct") == 0)
	{
		numSets = numLines;
		association = 1;
	}
	else if (strcmp(associativity, "assoc") == 0)
	{
		association = numLines;
		numSets = 1;
	}
	else
	{
		char* numString = malloc(sizeof(char)*strlen(associativity)-5);
		strncpy(numString, associativity+6, strlen(associativity)-5);
		association = atoi(numString);
		numSets = numLines/association;
		free(numString);
	}
	
	setIndexBitCalculator = numSets;
	while (setIndexBitCalculator>1)
	{
		setIndexBitCalculator = setIndexBitCalculator/2;
		*setIndexBits+=1;
	}
	
	while (blockSize>1)
	{
		blockSize >>=1;
		*blockOffsetBits ++;
	}
	int i, j;
	Cache myCache = makeItRain(numSets, association, replacementPolicy, writePolicy);
	myCache->setIndexNum = *setIndexBits;
	myCache->tagNum = 32 - (*setIndexBits + *blockOffsetBits);

	char* instructionPointer = (char*)malloc(sizeof(char)*12);
	char* instruction = (char*)malloc(sizeof(char)*2);
	char* hexVal = (char*)malloc(sizeof(char)*11);
	int count = 1;
while(fscanf(inputFile, "%s %s %s", instructionPointer, instruction, hexVal)!=EOF&&instructionPointer[0]!='#')
{
		{
		if (*instruction == 'R')
		{
		readFromCache(hexVal, myCache);
		}
		else if (*instruction == 'W')
		{
			writeToCache(hexVal, myCache);
		}
		}
}
    free(instructionPointer);
	free(instruction);
	free(hexVal);
	fclose(inputFile);
	printValues(myCache);
	freeEverything(myCache);
	free (setIndexBits);
	free (blockOffsetBits);
}
