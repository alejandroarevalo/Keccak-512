/*
 * utils.h - Dependency of keccak.h
 *
 *  Created on: Nov 26, 2014
 *      Author: Alejandro Arevalo
 */

#ifndef UTILS_H_
#define UTILS_H_

int_8* padding(int_8* message, int_32* size, int_32 bitRate);
int_32 mod (int_32 a, int_32 b);
int_64 rotate(int_64 x, int_8 numBits);

#endif /* UTILS_H_ */
