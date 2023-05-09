//bruteforceNaive.c
//Tambien cifra un texto cualquiera con un key arbitrario.
//OJO: asegurarse que la palabra a buscar sea lo suficientemente grande
//  evitando falsas soluciones ya que sera muy improbable que tal palabra suceda de
//  forma pseudoaleatoria en el descifrado.
//>> mpicc bruteforce.c -o desBrute
//>> mpirun -np <N> desBrute

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/des.h>

void long_to_bytes(long input, unsigned char *output) {
    for (int i = 7; i >= 0; i--) {
        output[i] = input & 0xFF;
        input >>= 8;
    }
}

//descifra un texto dado una llave
void decrypt(long mykey, char *ciph, int len){
  unsigned char key_bytes[8];
  long_to_bytes(mykey, key_bytes);
  DES_cblock key;
  memcpy(key, key_bytes, 8);
  DES_key_schedule key_schedule;
  DES_set_key_unchecked(&key, &key_schedule);

  DES_ecb_encrypt((const_DES_cblock*)ciph, (const_DES_cblock*)ciph, &key_schedule, DES_DECRYPT);
}

//cifra un texto dado una llave
void encrypt(long mykey, char *ciph){
  unsigned char key_bytes[8];
  long_to_bytes(mykey, key_bytes);
  DES_cblock key;
  memcpy(key, key_bytes, 8);
  DES_key_schedule key_schedule;
  DES_set_key_unchecked(&key, &key_schedule);

  DES_ecb_encrypt((const_DES_cblock*)ciph, (const_DES_cblock*)ciph, &key_schedule, DES_ENCRYPT);
}

//palabra clave a buscar en texto descifrado para determinar si se rompio el codigo
char search[] = "una prueba de";


int tryKey(long mykey, char *ciph, int len){
  unsigned char key_bytes[8] = {0};
    for (int j = 0; j < 8; ++j)
    {
        key_bytes[j] = (mykey >> j) & 0x01;
    }

  DES_key_schedule key_schedule;
  DES_set_key_unchecked((const_DES_cblock*)key_bytes, &key_schedule);


    // Decrypt the padded message
    unsigned char output[len];
    for (size_t i = 0; i < len; i += 8)
    {
     DES_ecb_encrypt((const_DES_cblock*)(ciph + i), (const_DES_cblock*)(ciph + i), &key_schedule, DES_DECRYPT);
    }

        // Check if decrypted plaintext contains known substring
        if (strstr((char *)ciph, search) != NULL)
        {
            // Print key and decrypted plaintext
            printf("\nKey: ");
            for (int k = 0; k < 8; ++k)
            {
                printf("%02x", key_bytes[k]);
            }
            printf("\n");
        }


  return strstr((char *)ciph, search) != NULL;
}

unsigned char message[] = "Esta es una prueba de proyecto 2";

long the_key = 123456L;
//2^56 / 4 es exactamente 18014398509481983
//long the_key = 18014398509481983L;
//long the_key = 18014398509481983L +1L;

int main(int argc, char *argv[]){ //char **argv
  long found = 0L;

  // Pad the message with null bytes if it is not a multiple of 8 bytes
  size_t message_len = strlen(message);
  size_t padded_len = message_len + (8 - message_len % 8);
  unsigned char* padded_message = (unsigned char*)calloc(padded_len, sizeof(unsigned char));
  memcpy(padded_message, message, message_len);

  // Encrypt the padded message
  encrypt(the_key, padded_message);

  
    // Print the encrypted ciphertext
    printf("Encrypted ciphertext: ");
    for (size_t i = 0; i < padded_len; ++i)
    {
        printf("%02x", padded_message[i]);
    }



  for(long i = 1; i<256; ++i){
    if(tryKey(i, padded_message, padded_len)){
      found = i;
      printf("Process %d found the key\n", i);
      break;
    }
  }


  printf("Key = %li\n\n", found);
  printf("%s\n", padded_message);
}
