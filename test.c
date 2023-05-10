#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <openssl/des.h>
#include <openssl/rand.h>

#define KEY_SPACE_SIZE 4294967296 // 2^32 possible keys

void long_to_bytes(long input, unsigned char *output)
{
	for (int i = 7; i >= 0; i--)
	{
		output[i] = input & 0xFF;
		input >>= 8;
	}
}

// descifra un texto dado una llave
void decrypt(long mykey, char *ciph, int len, unsigned char *iv)
{
	unsigned char key_bytes[8];
	long_to_bytes(mykey, key_bytes);
	DES_cblock key;
	memcpy(key, key_bytes, 8);
	DES_key_schedule key_schedule;
	DES_set_key_unchecked(&key, &key_schedule);

	DES_cblock ivec;
	memcpy(ivec, iv, sizeof(DES_cblock));

	DES_ncbc_encrypt(ciph, ciph, len, &key_schedule, &ivec, DES_DECRYPT);
}

// cifra un texto dado una llave
void encrypt(long mykey, char *ciph, int len, unsigned char *iv)
{
	unsigned char key_bytes[8];
	long_to_bytes(mykey, key_bytes);
	DES_cblock key;
	memcpy(key, key_bytes, 8);
	DES_key_schedule key_schedule;
	DES_set_key_unchecked(&key, &key_schedule);

	DES_cblock ivec;
	memcpy(ivec, iv, sizeof(DES_cblock));

	DES_ncbc_encrypt(ciph, ciph, len, &key_schedule, &ivec, DES_ENCRYPT);
}

// palabra clave a buscar en texto descifrado para determinar si se rompio el codigo
char search[] = "hello";

int tryKey(long initial_guess, char *ciph, int len, unsigned char *iv)
{
	for (long key_guess = initial_guess; key_guess < KEY_SPACE_SIZE; ++key_guess)
	{
		unsigned char *decrypted = (unsigned char *)calloc(len, sizeof(unsigned char));
		memcpy(decrypted, ciph, len);

		decrypt(key_guess, decrypted, len, iv);
		// Check if the decrypted message contains the plaintext
		if (strstr((char *)decrypted, search) != NULL)
		{
			memcpy(ciph, decrypted, len);
			return 1;
		}

		free(decrypted);
	}
	return 0;
}

unsigned char message[] = "esto es un texto mas largo hello";
long the_key = 123456L;

int main(int argc, char *argv[])
{
	long found = 0L;
	unsigned char iv[8];

	// Generate a 8-byte IV
	if (RAND_bytes(iv, 8) != 1)
	{
		printf("Error generating random bytes.\n");
		return 1;
	}

	// Pad the message with null bytes if it is not a multiple of 8 bytes
	size_t message_len = strlen(message);
	size_t padded_len = message_len + (8 - message_len % 8);
	unsigned char *padded_message = (unsigned char *)calloc(padded_len, sizeof(unsigned char));
	memcpy(padded_message, message, message_len);

	// Encrypt the padded message
	encrypt(the_key, padded_message, padded_len, iv);
	// Print the encrypted ciphertext
	printf("Encrypted ciphertext: ");
	for (size_t i = 0; i < padded_len; ++i)
	{
		printf("%02x", padded_message[i]);
	}

	// Decrypt the padded message
	double time_spent = 0.0;
	clock_t initial, end;
	initial = clock();
	if (tryKey(found, padded_message, padded_len, iv))
	{
		printf("\nOutput luego del trykey: %s\n", padded_message);
	}
	end = clock();
	time_spent += (double)(end - initial) / CLOCKS_PER_SEC;
	printf("Time to find Key: %f", time_spent);
}
