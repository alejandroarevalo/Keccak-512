/*
 * keccak.c
 *
 *  Created on: Nov 21, 2014
 *      Author: Alejandro Arevalo
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keccak.h"
#include "utils.h"

const int_64 ROUND_CONSTANTS[24]={
		0x0000000000000001,
		0x0000000000008082,
		0x800000000000808A,
		0x8000000080008000,
		0x000000000000808B,
		0x0000000080000001,
		0x8000000080008081,
		0x8000000000008009,
		0x000000000000008A,
		0x0000000000000088,
		0x0000000080008009,
		0x000000008000000A,
		0x000000008000808B,
		0x800000000000008B,
		0x8000000000008089,
		0x8000000000008003,
		0x8000000000008002,
		0x8000000000000080,
		0x000000000000800A,
		0x800000008000000A,
		0x8000000080008081,
		0x8000000000008080,
		0x0000000080000001,
		0x8000000080008008
};

const int_64 ROTATION_OFFSETS[5][5]={
		{0	,36	,3	,41	,18	},
		{1	,44	,10	,45	,2	},
		{62	,6	,43	,15	,61	},
		{28	,55	,25	,21	,56	},
		{27	,20	,39	,8	,14	}
};

int_64** initialize(int_8* message, int_32* messageSize, int_32 bitRate);
int_64** absorbingPhase(int_64** keccakState, int_64 *message64, int_32* messageSize, int_32 bitRate, int_32 laneLength);

int_64** keccakFunction(int_64** keccakState);
int_64** keccakRound(int_64 ** keccakState, int_64 roundConstant);

int_8* squeezingPhase(int_64** keccakState, int_32 bitRate, int_32 laneLength);

//---------------------------------------DEBUG ONLY---------------------------------------
void printMatr(int_64 **keccakState, int_32 step);
//----------------------------------------------------------------------------------------

int_8* keccak(int_8* message, int_32* messageSize, int_32 bitRate, int_32 laneLength){

	//---PADDING THE MESSAGE---
	if((*messageSize % bitRate) != 0){
		message = padding(message, messageSize, bitRate);
	}
	int_64 *message64 = (int_64 *)message;

	//---INITIAL STATE---
	int_64** keccakState = initialize(message, messageSize, bitRate);

	//---ABSORBING PHASE---
	keccakState = absorbingPhase(keccakState, message64, messageSize, bitRate, laneLength);

	//---SQUEZZING PHASE---
	int_8* result = squeezingPhase(keccakState, bitRate, laneLength);

	return result;
}

int_64** initialize(int_8* message, int_32* messageSize, int_32 bitRate){

	//Creating an matrix of 5*5, where each element has 64 bit length called a LANE.
	int_64** keccakState = (int_64**) calloc(5, sizeof(int_64*));
	for(int_64 i = 0; i < 5; i++)
		keccakState[i] = (int_64*) calloc(5, sizeof(int_64));

	for(int_64 i = 0; i < 5; i++)
		for(int_64 j = 0; j < 5; j++)
			keccakState[i][j] = 0x00;

	return keccakState;

}

int_64** absorbingPhase(int_64** keccakState, int_64 *message64, int_32* messageSize, int_32 bitRate, int_32 laneLength){
	int_32 rate = bitRate/laneLength;

	//After the padding, the messageSize is a multiple of bitRate.
	for(int_32 i=0; i < ((*messageSize)/bitRate) ; i++){

		for(int_32 j=0; j<5; j++){
			if(5*j >= rate)
				continue;

			for(int_32 x=0;x<5;x++){
				if((x+5*j) < rate){
					keccakState[x][j] = keccakState[x][j] ^ *(message64 + (i*rate + x + 5*j));
				}
			}
		}

		//TEST-ONLY
//		printMatr(keccakState, i);
		//END TEST-ONLY

		keccakState = keccakFunction(keccakState);

		//TEST-ONLY
//		printMatr(keccakState, i + 1);
		//END TEST-ONLY
	}

	return keccakState;
}

int_8* squeezingPhase(int_64** keccakState, int_32 bitRate, int_32 laneLength){
	int_32 b=0;
	int_32 rate = bitRate/laneLength;

	int_64 *Z=(int_64 *)calloc(8,sizeof(int_64));

	while(b < 8){
		for(int_32 y=0;y<5;y++){
			if(5*y >= rate)
				continue;

			for(int_32 x=0;x<5;x++){

				if((x+5*y) < rate){

					*(Z+b) ^= keccakState[x][y];
					b++;

				}
			}
		}
		keccakState = keccakFunction(keccakState);
	}
	return (int_8*) Z;
}

int_64** keccakFunction(int_64** keccakState){
	//The number of rounds is calculated using NumberRounds = 12+2l, where power(2,l) = w,
	//since our w = 64 (the length of our Lane, that's why the int_64), then we have l=6, and NumberRounds = 24

	for(int_32 i=0; i < 24; i++){
		keccakState = keccakRound(keccakState, ROUND_CONSTANTS[i]);
	}

	return keccakState;
}

int_64** keccakRound(int_64 ** keccakState, int_64 roundConstant){
	int_64 C[5];
	int_64 D[5];
	int_64 B[5][5];

	//---THETA step---
	for(int_8 x = 0; x < 5 ; x++ ){
		C[x] = keccakState[x][0] ^ keccakState[x][1] ^ keccakState[x][2]^ keccakState[x][3] ^ keccakState[x][4];
	}
	for(int_8 x=0; x < 5 ; x++ ){
		D[x] = C[mod((x - 1), 5)] ^ rotate(C[ mod((x + 1), 5)], 1);
	}
	for(int_8 x = 0; x < 5 ; x++ ){
		for(int_8 y=0;y<5;y++){
			keccakState[x][y] = keccakState[x][y] ^ D[x];
		}
	}

	//---RHO & PHI steps---
	for(int_8 x=0;x<5;x++){
		for(int_8 y=0;y<5;y++){
			B[y][mod((2*x+3*y), 5)] = rotate(keccakState[x][y], ROTATION_OFFSETS[x][y]);
		}
	}

	//---XI step---
	for(int_8 x=0;x<5;x++){
		for(int_8 y=0;y<5;y++){
			keccakState[x][y] = B[x][y]^((~B[mod((x+1),5)][y]) & B[mod((x+2),5)][y]);
		}
	}

	//---IOTA step---
	keccakState[0][0] = keccakState[0][0] ^ roundConstant;

	return keccakState;
}



//---------------------------------------DEBUG ONLY---------------------------------------
void printMatr(int_64 **keccakState, int_32 step){
	if(step == 0){
		printf("---------------STEP[%d]---------------\n STRING\n", step + 1);
		for(int_32 y=0;y<5;y++){
			for(int_32 x=0;x<5;x++){

				if(x == 0) printf("[");

				printf("%s", &(keccakState[x][y]));

				if(x >= 0 && x < 4){
					printf(", ");
				}else{
					printf("]\n");
				}

			}
		}
	}

	printf("HEX\n");
	for(int_32 y=0;y<5;y++){
		for(int_32 x=0;x<5;x++){
			if(x == 0) printf("[");

			printf("%.8X", keccakState[x][y]);

			if(x >= 0 && x < 4){
				printf(", ");
			}else{
				printf("]\n");
			}
		}
	}
}
