# CPD_Proyecto2

## Proyecto

```
mpicc bruteforceNaive.c -o bruteforceNaive -lssl -lcrypto
mpiexec -n 1 ./bruteforceNaive
```

## Tests
```
gcc test.c -o test -lssl -lcrypto
./test
```
```
gcc test2.c -o test -lssl -lcrypto
./test
```