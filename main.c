/*
 * main.c
 *
 * Basically this implementation of keccak, works with the fixed output length of 512 Keccak[r = 576, c = 1024]
 * This implementation has been made by myself only for better understanding of the Keccak algorithm, therefore
 *
 *
 *  Created on: Nov 20, 2014
 *      Author: Alejandro Arevalo
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keccak.h"


//The bit rate represents the bits of the state that are updated during the sponge construction
//This value is given in "BYTES" so the number of BITS updated during the sponge construction is the 576.
int_32 BIT_RATE = 72;

//This value can be of {1, 2, 4, 8, 16, 64} "BITS",
//in this implementation the length is represented in "BYTES", then 8 bytes = 64 bit
int_32 LANE_LENGTH = 8;//8 bytes = 64 bits

int main (int argc, char *argv[]){
	char* message = NULL;

	if(argc > 1)
		message = argv[1];
	else
		message = "pruebitamuylarguisimalamaslargadelmundoestasiessuperlargalarguisimalatengoquehacermaslargasoloparahaceresapruebafea";


	int_32 size = strlen(message);

	int_8* result = keccak(message, &size, BIT_RATE, LANE_LENGTH);

	for(int i = 0; i < 64; i++){
		int_8  num = *(result+i);
		printf("%.2X", num);
	}

}
