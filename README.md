# CPD_Proyecto2

## Proyecto

```
  mpic++ ./brute1.cpp -lssl -lcrypto -o brute1.exe
  mpirun --use-hwthread-cpus --oversubscribe -np 4 ./brute1.exe 120
```
or
```
mpic++ -static-libgcc -static-libstdc++ -lmingwex ./brute1.cpp -lssl -lcrypto -o brute1.exe
mpiexec -n 4 ./brute1.exe 120
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