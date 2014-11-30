/*
 * utils.c
 *
 *  Created on: Nov 25, 2014
 *      Author: Alejandro Arevalo
 */
#include <stdio.h>
#include <stdlib.h>
#include "keccak.h"
#include "utils.h"


/**
 *
 */
int_8* padding(int_8* message, int_32* size, int_32 bitRate){

	int_32 tempSize = *size;
	int_32 newSize = (tempSize + bitRate - (tempSize % bitRate));

	int_8 *newMessage;
	newMessage = malloc(newSize);

	//The new string will contain Message || 0x01 || 0x00 || … || 0x00
	for(int_32 j=0; j < tempSize; j++){
		*(newMessage+j) = *(message+j);
	}

	*(newMessage+tempSize)=0x01;
	tempSize++;

	while(tempSize < (newSize-1)){
		*(newMessage+tempSize) = 0x00;
		tempSize++;
	}

	*(newMessage+tempSize)=0x80;

	tempSize++;
	*size=tempSize;
	return newMessage;
}

int_32 mod (int_32 a, int_32 b){
	if(b < 0)
		return mod(-a, -b);
	int res = a % b;
	if(res < 0)
		res+=b;
	return res;
}

//In order to make a cycle rotation
int_64 rotate(int_64 x, int_8 numBits){
	return ((x << numBits) | (x >> (64 - numBits)));
}
