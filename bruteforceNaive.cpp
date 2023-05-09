//bruteforceNaive.c
//Tambien cifra un texto cualquiera con un key arbitrario.
//OJO: asegurarse que la palabra a buscar sea lo suficientemente grande
//  evitando falsas soluciones ya que sera muy improbable que tal palabra suceda de
//  forma pseudoaleatoria en el descifrado.
//>> mpic++ ./bruteforceNaive.cpp -lssl -lcrypto -o desBrute.exe
//>> mpirun -np 2 desBrute.exe

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <fstream>
#include <openssl/des.h>

using namespace std;

#define KEY_SPACE_SIZE 4294967296 // 2^32 possible keys

void long_to_bytes(long input, unsigned char *output) {
  for (int i = 7; i >= 0; i--) {
    output[i] = input & 0xFF;
    input >>= 8;
  }
}

//descifra un texto dado una llave
void decrypt(long mykey, unsigned char *ciph, int len, unsigned char *iv)
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

//cifra un texto dado una llave
void encrypt(long mykey, unsigned char *ciph){
  unsigned char key_bytes[8];
  long_to_bytes(mykey, key_bytes);
  DES_cblock key;
  memcpy(key, key_bytes, 8);
  DES_key_schedule key_schedule;
  DES_set_key_unchecked(&key, &key_schedule);

  DES_ecb_encrypt((const_DES_cblock*)ciph, (const_DES_cblock*)ciph, &key_schedule, DES_ENCRYPT);
}

//palabra clave a buscar en texto descifrado para determinar si se rompio el codigo
char search[] = "es una prueba de";
int tryKey(long initial_guess, unsigned char *ciph, int len, unsigned char *iv) {
	for (long key_guess = initial_guess; key_guess < KEY_SPACE_SIZE; ++key_guess)
	{
		unsigned char *decrypted = (unsigned char *)calloc(len, sizeof(unsigned char));
		memcpy(decrypted, ciph, len);
		decrypt(key_guess, decrypted, len, iv);

		// Check if the decrypted message contains the plaintext
		if (strstr((char *)decrypted, search) != NULL) {
			memcpy(ciph, decrypted, len);
			return 1;
		}

		free(decrypted);
	}
	return 0;
}

void Build_mpi_type(double* a_p, MPI_Datatype* input_mpi_t_p) {
  MPI_Aint array_of_displacements[3] = {0};
  MPI_Aint a_addr, b_addr, n_addr;

  MPI_Get_address(a_p, &a_addr);

  array_of_displacements[1] = n_addr-b_addr;
  array_of_displacements[2] = a_addr-b_addr; 

  int array_of_blocklengths[3] = {1, 1, 1};
  MPI_Datatype array_of_types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
  
  MPI_Type_create_struct(
    3,
    array_of_blocklengths, 
    array_of_displacements,
    array_of_types,
    input_mpi_t_p
  );
  MPI_Type_commit(input_mpi_t_p);
  
}  /* Build_mpi_type */

/**
 * Gets the key that the user will do as input
 * @param my_rank the id of the process
 * @param comm_sz the context in which they are running
 * @param tstart the timer, will start when the user gives the input (out)
 * @param message_len the lenght of the message encrypted (out)
 * @param message the encrypted message (out)
*/
void Get_input(int my_rank, int comm_sz, double* tstart, size_t* message_len, unsigned char* padded_message) {
  MPI_Datatype input_mpi_t;

  // Build_mpi_type(a_p, b_p, n_p, &input_mpi_t);

  if (my_rank == 0) {
    printf("Enter a, b, and n\n");
    scanf("%lf %lf %d", a_p, b_p, n_p);
    *tstart = MPI_Wtime();
  }

  MPI_Bcast(b_p, 1, input_mpi_t, 0, MPI_COMM_WORLD);
  MPI_Type_free(&input_mpi_t);
}

/**
 * Gets the content of a file
 * @param path location of the file to read
 * @returns the content of the file
*/
string getFileBody (string path) {
  int exists = 0;
  string data, aux;
  ifstream file(path);

  if (file.fail()) {
    cout << "[File] Error reading file " << path << endl;
    MPI_Finalize();
    exit(-1);
  }

  while (getline(file, aux)) {
    data += aux;
    exists = 1;
    if (file.peek() != EOF) {
      data += "\n";
    }
  }

  if (exists == 0) {
    cout << "[File] Empety file encountered" << endl;
    MPI_Finalize();
    exit(-1);
  }

  file.close();
  return data;
}

long the_key = 123456L;
//2^56 / 4 es exactamente 18014398509481983
//long the_key = 18014398509481983L;
//long the_key = 18014398509481983L +1L;

int main(int argc, char *argv[]){ //char **argv
  int N, id;
  long upper = (1L << 56); //upper bound DES keys 2^56
  long mylower, myupper;
	unsigned char iv[8];
  unsigned char* padded_message;
  size_t padded_len;

  MPI_Status st;
  MPI_Request req;
  MPI_Comm comm = MPI_COMM_WORLD;

  //INIT MPI
  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &N);
  MPI_Comm_rank(comm, &id);

  long found = 0L;
  int ready = 0;

  // Reads the file and encrypts the message (it will print the encirpted message)
  if (id == 0) {
    string fileBody = getFileBody("./data.txt");

    size_t message_len = fileBody.size();
    unsigned char* message[message_len];

    strcpy((char*) message, fileBody.data());

    padded_len = message_len + (8 - message_len % 8);
    padded_message = (unsigned char*)calloc(padded_len, sizeof(unsigned char));
    memcpy(padded_message, message, message_len);
    encrypt(the_key, padded_message);

    cout << "The encripted message looks like:" << endl;
    for (size_t i = 0; i < padded_len; ++i) {
      printf("%02x", padded_message[i]);
    }
    cout << endl;
  }

  // Distributes the work
  long range_per_node = upper / N;
  mylower = range_per_node * id;
  myupper = range_per_node * (id + 1) -1;

  if(id == N - 1){
    myupper = upper;
  }
  printf("Process %d lower %ld upper %ld\n", id, mylower, myupper);

  // non blocking receive, revisar en el for si alguien ya encontro
  MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);

  for(long i = mylower; i<myupper; ++i){
    MPI_Test(&req, &ready, MPI_STATUS_IGNORE);
    if (ready)
      break;  //ya encontraron, salir

    if (tryKey(i, padded_message, padded_len, iv)) {
      found = i;
      printf("Process %d found the key\n", id);
      for(int node=0; node<N; node++){
        MPI_Send(&found, 1, MPI_LONG, node, 0, comm); //avisar a otros
      }
      break;
    }

  }

  // //wait y luego imprimir el texto
  // if(id==0){
  //   MPI_Wait(&req, &st);
  //   decrypt(found, cipher, ciphlen);
  //   printf("Key = %li\n\n", found);
  //   printf("%s\n", cipher);
  // }

  printf("Process %d exiting\n", id);

  // FIN entorno MPI
  MPI_Finalize();
}
