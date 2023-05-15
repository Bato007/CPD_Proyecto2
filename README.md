# CPD_Proyecto2

## Proyecto

```
  mpic++ ./bruteforceNaive.cpp -lssl -lcrypto -o desBrute.exe
  mpirun --use-hwthread-cpus --oversubscribe -np 4 ./desBrute.exe 120
```

## Secuenciales
```
  gcc seq.c -o seq -lssl -lcrypto
  ./seq
```
```
  gcc seq2.c -o seq -lssl -lcrypto
  ./seq
```
```
  gcc seq3.c -o seq -lssl -lcrypto
  ./seq
```