/*
 * keccak.h
 *
 *  Created on: Nov 24, 2014
 *      Author: Alejandro Arevalo
 */

#ifndef KECCAK_H_
#define KECCAK_H_

typedef unsigned char 		int_8;	//Integer represented by 8 bits.
typedef int  				int_32;	//Integer represented by 32 bits.
typedef unsigned long int	int_64;	//Integer represented by 64 bits.

int_8* keccak(int_8* message, int_32* messageSize, int_32 bitRate, int_32 laneLength);

#endif /* KECCAK_H_ */
