#include <stm32f4xx.h>
#include <stdio.h>
#include <stdlib.h>
#include "EncrypteMessages.h"


//Encrypt and decrypt should be able to handle > d4 bit words.
void encryptWrapper(char* d, uint32_t* key);
void decryptWrapper(char* d, uint32_t* key, int loops);
void encryptAssemblyWrapper(char* d, uint32_t* key);
void decryptAssemblyWrapper(char* d, uint32_t* key);
void decryptTAWrapper(void);
void encrypt (uint32_t* d, uint32_t* k);
void decrypt (uint32_t* d, uint32_t* k);

//! \brief A wrapper for the C encryption function.
/*
  A wrapper for the C encryption function. It loops
  over the entire string.
  \param d the string to be encoded
	\param key the keys to be used in the encryption 
*/
void encryptWrapper(char* d, uint32_t* key){
	uint32_t loops = strlen(d)/8, i;
	for(i=0; i<loops; i++){
			encrypt((uint32_t*)d+2*i, key);
		}
}

//! A C implementation of the TEA encryption.
/*
  \param d the 8 characters to be encoded
	\param key the keys to be used in the encryption
*/
void encrypt (uint32_t* d, uint32_t* k){
	//Pseudocode given to us. Basic implementation of it.
		uint32_t* d0 = d;
		uint32_t* d1 = d+ 1;
		uint32_t sum = 0, i;
    uint32_t delta = 0x9E3779B9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for(i=0; i < 32; i++){
        sum += delta;
        *d0 += ((*d1<<4) + k0) ^ (*d1 + sum) ^ ((*d1>>5) + k1);
        *d1 += ((*d0<<4) + k2) ^ (*d0 + sum) ^ ((*d0>>5) + k3);
    }
}
//! \brief A wrapper for the C decryption function.
/*
  A wrapper for the C decryption function. It loops
  over the entire set of encrypted characters.
  \param d the string to be decoded
	\param key the keys to be used in the decryption 
*/
 void decryptWrapper(char* d, uint32_t* key, int loops){
	uint32_t loop = loops, i;
	for(i=0; i<loop; i++){
			decrypt((uint32_t*)d+2*i, key);
		}
}

//! A C implementation of the TEA decryption.
/*
  \param d the 8 characters to be decrypted
	\param key the keys to be used in the decryption
*/
void decrypt (uint32_t* d, uint32_t* k){
	//Pseudocode given to us. Basic implementation of it.
		uint32_t* d0 = d;
		uint32_t* d1 = d+1;
    uint32_t sum = 0xC6EF3720, i;
    uint32_t delta = 0x9E3779B9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for(i=0; i < 32; i++){
        *d1 -= ((*d0<<4) + k2) ^ (*d0 + sum) ^ ((*d0>>5) + k3);
        *d0 -= ((*d1<<4) + k0) ^ (*d1 + sum) ^ ((*d1>>5) + k1);
        sum -= delta;
    }
}

//! \brief A special wrapper for the C decryption function.
/*
	
  This wrapper finds the key for the Secret Message for 
	group 11, and then decrypts the message in place.
*/
void decryptTAWrapper(void){
	uint32_t i, j, k, test;
	uint32_t decryptKeys[2];
	uint32_t testWords[2];
	uint32_t encryptKeys[] = {3, 5, 9, 4};
	decryptKeys[0] = 'T';
	decryptKeys[1] = 'A';
	
	//First encrypts the characters T and A with the given code.
	encrypt(decryptKeys, encryptKeys);
	//Sets the results of the encryption as the first two keys in the decrypt array.
	encryptKeys[0] = decryptKeys[0];
	encryptKeys[1] = decryptKeys[1];
	//Loops through all possible combinations of keys until the correct combination is found,
	//then decrypts entire message with correct key.
	for(i=0x64; i < 0x69; i++){
		for(j=0x64; j < 0x69; j++){
			//To test for correctness, it hardcodes the first two words of our message and attempts
			//to decrypt them
			testWords[0] = 0x560a4211;
			testWords[1] = 0x41433e19;
			encryptKeys[2] = i;
			encryptKeys[3] = j;
			decrypt(testWords, encryptKeys);
			
			//It tests for correctness based off of how characters are handled in hex.
			//A single character will only occupy the last two digits, so any number
			//of the form 0x000000XX can be expected to be an ASCII number, as long as
			//it is between 0 and 7F.
			test = testWords[0];
			test = test & 0xFFFFFF00;
			if(test == 0x00000000 && test <= 0x0000007F){
				for(k=0; k<13; k++){
					decrypt(Secret_Quote_Group_11+2*k, encryptKeys);
				}
				return;
			}
		}
	}
}

int main(void){
	//Still to do:
				//Comment
				//Optimize?
	//char inputMessage[] = {"It began with the forging of the great rings. Three were given to the Elves: Immortal, wisest and fairest of all beings. Seven to the dwarf lords: Great miners and craftsmen of the mountain halls. And nine, nine rings were gifted to the race of men who, above all else, desire power. For within these rings was bound the strength and the will to govern each race. But they were, all of them, deceived; for another ring was made. In the land of Mordor, in the fires of Mount Doom, the Dark Lord Sauron forged, in secret, a master ring. And into this ring he poured his cruelty, his malice and his will to dominate all life. One ring to rule them all. One by one, the free lands of middle-earth fell to the power of the ring.     "};
	char inputMessage[] = {"Space: the final frontier. These are the voyages of the starship Enterprise. Its continuing mission to explore strange new worlds, to seek out new life and new civilizations, to boldly go where no one has gone before.       "};
	int loop = strlen(inputMessage)/8;
	uint32_t k[] = {3, 5, 9, 4};
	encryptWrapper(inputMessage, k);
 	decryptWrapper(inputMessage, k, loop);
 	encryptAssemblyWrapper(inputMessage, k);
 	decryptAssemblyWrapper(inputMessage, k);
	decryptTAWrapper();
	return 1;
}
