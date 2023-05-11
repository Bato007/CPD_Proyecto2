# CPD_Proyecto2

## Proyecto

```
  mpic++ ./bruteforceNaive.cpp -lssl -lcrypto -o desBrute.exe
  mpirun --use-hwthread-cpus --oversubscribe -np 4 ./desBrute.exe 120
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