#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>




char* hexToBinary(char* hexVal)
{
	//char* binaryNum = malloc(sizeof(char)*33);
	char binaryNum [33];
	binaryNum[0] = '\0';
	char* binaryFinal = malloc(sizeof(char)*33);
	int i;
	int j;
	char curr;
	for (i=strlen(hexVal)-1; i>1; i--)
	{
		curr = hexVal[i];
		switch (curr)
		{	
			case '0': strcat(binaryNum, "0000"); break;
			case '1': strcat (binaryNum, "1000"); break;
			case '2': strcat (binaryNum, "0100");break;
			case '3':strcat (binaryNum, "1100");break;
			case '4':strcat(binaryNum, "0010");break;
			case '5':strcat (binaryNum, "1010");break;
			case '6':strcat(binaryNum, "0110");break;
			case '7':strcat(binaryNum, "1110");break;
			case '8':strcat(binaryNum, "0001");break;
			case '9':strcat(binaryNum, "1001");break;
			case 'a':strcat (binaryNum, "0101");break;
			case 'b':strcat(binaryNum, "1101");break;
			case 'c':strcat(binaryNum, "0011");break;
			case 'd':strcat(binaryNum, "1011");break;
			case 'e':strcat (binaryNum, "0111");break;
			case 'f':strcat(binaryNum, "1111"); break;
		}
	}

	for (i=strlen(binaryNum); i<32; i++)
	{
		strcat(binaryNum, "0");
	}
	i = 0;
	for (j=strlen(binaryNum)-1; j>=0; j--)
	{
		binaryFinal[i] = binaryNum[j];
		i++;
	}
	return binaryFinal;
}

struct block
{
	//printf("%s", myCache->cacheTable[0][0]->tag);
	int dirty;
	int valid;
	char* tag;
	int replacement;
};typedef struct block* Block;
struct cache
{
	int sets;
	int associativity;
	int readNum;
	int writeNum;
	int hitNum;
	int missNum;
	int setIndexNum;
	int tagNum;
	char* replacePolicy;
	char* writePolicy;
	Block** cacheTable;
};
typedef struct cache* Cache;
//initializes an empty cache with valid bit of all blocks set to 0 to indicate invalid, and tags set to NULL to indicate empty
void printValues(Cache cache)
{
	printf("READS: %i\nWRITES:%i\nHITS:%i\nMISSES:%i\n", 
	cache->readNum, cache->writeNum, cache-> hitNum, cache->missNum);
}


Cache makeItRain(int setNum, int blockNum, char* replacePolicy, char* writePolicy)
{
	int i;
	int j;
	int dimension = setNum*blockNum;
	Cache newCache = (Cache)malloc(sizeof(struct cache));
	Block** cacheTable = (Block**)malloc(sizeof(Block*)*setNum);
	for (i=0; i<setNum; i++)
	{
		cacheTable[i]= (Block*)malloc(sizeof(Block)*blockNum);
		for (j=0; j<blockNum; j++)
		{
			cacheTable[i][j] = (Block)malloc(sizeof(struct block));
		}
	}
	for (i=0; i<setNum; i++)
	{
		for (j=0; j<blockNum; j++)
		{
			cacheTable[i][j]->dirty = 0;
			cacheTable[i][j]->valid = 0;
			cacheTable[i][j]->tag = (char*)malloc(sizeof(char)*33);
			cacheTable[i][j]->replacement = 0;
		}
	}
	newCache->cacheTable = cacheTable;
	//printf("inside makeItRain: valid: %i", new->cacheTable[i][j].valid);
	newCache->sets = setNum;
	newCache->associativity = blockNum;
	newCache->hitNum = 0;
	newCache->missNum = 0;
	newCache->readNum = 0;
	newCache->writeNum = 0;
	newCache->replacePolicy = replacePolicy;
	newCache->writePolicy = writePolicy;
	return newCache;
}


char* getTag(char* hexValue, Cache cache)
{
	char* binaryNum=hexToBinary(hexValue);
	char* tag = (char*)malloc(sizeof(char)*33);
	int num = cache->tagNum;
	strncpy(tag, binaryNum, num);
	tag[num] = '\0';
	free(binaryNum);
	return tag;
}

int* getSetIndex(Cache cache, char* hexVal)
{
	int i;
	int* answer = (int*)malloc(sizeof(int));
	//*answer = 0;
	char setIndexBits[cache->setIndexNum+1];
	char* binaryNum = hexToBinary(hexVal);
	strncpy(setIndexBits, binaryNum+cache->tagNum, cache->setIndexNum);
	setIndexBits[cache->setIndexNum] = '\0';
	*answer =(int)strtol(setIndexBits, NULL, 2);
	/*for (i=0; setIndexRaw>1; i++)
	{
		*answer += 1;
	  	setIndexRaw = setIndexRaw/2;
	}*/
	free(binaryNum);
	return answer;
}

void readFromCache(char* hexVal, Cache cache)
{
	char* replace = cache->replacePolicy;
	int i;
	int j;
	int disowned;
	int openSpot =-1;
	char* tag = getTag(hexVal, cache);
	//printf("READ TAG:%s\n",tag); 
	//NEW
	int* setPointer = getSetIndex(cache,hexVal);
	//printf("SET INDEX: %i", *setPointer);
	int set =*setPointer;
	char* binaryNum = hexToBinary(hexVal);
	free (setPointer);
	printf("BINARY: %s TAG: %s, SET: %i\n", binaryNum, tag, set);
	for (i=0; i<cache->associativity; i++)
	{
		//if there's a hit
		if (cache->cacheTable[set][i]->valid == 1)
		{
		if (strcmp(cache->cacheTable[set][i]->tag, tag) ==0)
		{
			//printf("hit\n");
			cache->hitNum +=1;
			if (strcmp(cache->replacePolicy, "LRU")==0)
			{
				cache->cacheTable[set][i]->replacement = -1;
				//update the recency of all blocks in the set
				for (j=0; j<cache->associativity; j++)
				{
					cache->cacheTable[set][j]->replacement += 1;
				}
			}
			free(tag);
			return;
		}
		}
		//make note of open spaces if there's a miss
		if (cache->cacheTable[set][i]->valid == 0)
		{
			openSpot = i;
		}
	
	}			
	cache->missNum +=1;
	cache->readNum +=1;
	//put new address in open block if one exists, update recency/placement number for all blocks according to replacement policy
	if (openSpot != -1)
	{//printf("merp");
		cache->cacheTable[set][openSpot]->dirty = 0;
		cache->cacheTable[set][openSpot]->valid = 1;
		strcpy(cache->cacheTable[set][openSpot]->tag, tag);
		free(tag);
		//cache->cacheTable[set][openSpot]->tag = tag;
	 		cache->cacheTable[set][openSpot]->replacement = -1;
			for (j=0; j<cache->associativity; j++)
				{
					cache->cacheTable[set][j]->replacement += 1;
				}
			
				return;
	}
		
		//no open spot
		else
		{
			//printf("no open spot\n");
			disowned = 0;
			//find the block to be evicted
			for (i=1; i<cache->associativity; i++)
			{
				if (cache->cacheTable[set][i]->replacement > cache->cacheTable[set][disowned]->replacement)
				disowned = i;
			}
			if (cache->cacheTable[set][disowned]->dirty == 1)
			{
				//printf("write");
				cache->writeNum+=1;
			}
			cache->cacheTable[set][disowned]->dirty =0;
			strcpy(cache->cacheTable[set][disowned]->tag, tag);
			free(tag);
			//cache->cacheTable[set][disowned]->tag = tag;
			cache->cacheTable[set][disowned]->replacement = -1;
			for (i=0; i<cache->associativity; i++)
			{
				cache->cacheTable[set][i]->replacement ++;
			}
		}
	}

	void writeToCache(char* hexVal, Cache cache)
	{
		char* replace = cache->replacePolicy;
		int i;
		int j;
		int disowned;
		int openSpot =-1;
		char* tag = getTag(hexVal, cache);
	//	printf("TAG: %s\n", tag);
		//NEW
		int* setPointer = getSetIndex(cache, hexVal);
		int set = *setPointer;
	//	printf("SET INDEX: %i", *setPointer);		
		free(setPointer);
		for (i=0; i<cache->associativity; i++)
		{
		//if there's a hit
			if (cache->cacheTable[set][i]->valid == 1)
			{
				if (strcmp(cache->cacheTable[set][i]->tag, tag) ==0)
				{
					cache->hitNum +=1;
					free(tag);
					if (strcmp(cache->replacePolicy, "LRU")==0)
					{
						cache->cacheTable[set][i]->replacement = -1;
						//update the recency of all blocks in the set
						for (j=0; j<cache->associativity; j++)
						{
							cache->cacheTable[set][j]->replacement += 1;
						}
					}
					if (strcmp(cache->writePolicy, "wb") ==0)
					{
						cache->cacheTable[set][i]->dirty = 1;
					}
					if (strcmp(cache->writePolicy, "wt")==0)
					{
						//printf("write\n");
						cache-> writeNum+=1;
					}
					return;
					}
			}
			//make note of open spaces if there's a miss
			if (cache->cacheTable[set][i]->valid == 0)
			{
				openSpot = i;
			}	
		}			
		//printf("miss\n");
		cache->missNum +=1;
		//printf("read\n");
		cache->readNum +=1;
		//double check on this, not sure if you add to the writes for misses
		if(strcmp("wt", cache->writePolicy)==0)
		{
			//printf("write\n");
			cache->writeNum+=1;
		}
		//put new address in open block if one exists, update recency/placement number for all blocks according to replacement policy
		if (openSpot != -1)
		{//printf("merp");
			cache->cacheTable[set][openSpot]->valid = 1;
			strcpy(cache->cacheTable[set][disowned]->tag, tag);
			free(tag);
			//cache->cacheTable[set][openSpot]->tag = tag;
	 		cache->cacheTable[set][openSpot]->replacement = -1;
			cache->cacheTable[set][openSpot]->dirty = 0;
			for (j=0; j<cache->associativity; j++)
					{
						cache->cacheTable[set][j]->replacement += 1;
					}
					//free(tag);
					return;
		}
			
			//no open spot
			else
			{
				//printf("no open spot\n");
				disowned = 0;
				//find the block to be evicted
				for (i=1; i<cache->associativity; i++)
				{
					if (cache->cacheTable[set][i]->replacement > cache->cacheTable[set][disowned]->replacement)
					disowned = i;
				}
				//printf("TO BE EVICTED: %s\n", cache->cacheTable[set][disowned]->tag);
				if (strcmp("wb", cache->writePolicy)==0)
				{
					if(cache->cacheTable[set][disowned]->dirty == 1)
					{
						cache->writeNum+=1;
					}

				}
			/*	if (strcmp("wt", cache->writePolicy)==0 && cache->cacheTable[set][disowned]->dirty == 1)
				{
					cache->writeNum++;
				}*/
				cache->cacheTable[set][disowned]->valid  =1;
				cache->cacheTable[set][disowned]->dirty =0;
				strcpy(cache->cacheTable[set][disowned]->tag, tag);
				free(tag);
				//cache->cacheTable[set][disowned]->tag = tag;
				cache->cacheTable[set][disowned]->replacement = -1;
			//	printf("%s\n%s\n",cache->cacheTable[set][0]->tag, cache->cacheTable[set][1]->tag);
				for (i=0; i<cache->associativity; i++)
				{
					cache->cacheTable[set][i]->replacement ++;
				}
			}
		}

void freeEverything(Cache cache)
{
	int i, j;
	for (i=0; i<cache->sets; i++)
	{
		for (j=0; j<cache->associativity; j++)
		{
			free(cache->cacheTable[i][j]->tag);
			free (cache->cacheTable[i][j]);
		}
	}
	for (i=0; i<cache->sets; i++)
	{
		free(cache->cacheTable[i]);
	}
	free(cache->cacheTable);
	free(cache);
}



