# CPD_Proyecto2

## Proyecto

```
  mpic++ ./brute1.cpp -lssl -lcrypto -o brute1.exe
  mpirun --use-hwthread-cpus --oversubscribe -np 4 ./brute1.exe

  mpic++ ./brute2.cpp -lssl -lcrypto -o brute2.exe
  mpirun --use-hwthread-cpus --oversubscribe -np 4 ./brute2.exe

  mpic++ ./brute3.cpp -lssl -lcrypto -o brute3.exe
  mpirun --use-hwthread-cpus --oversubscribe -np 4 ./brute3.exe
```

## Secuenciales
```
  g++ seq.cpp -o seq -lssl -lcrypto -fpermissive
  ./seq | ./seq `key`
```
```
  g++ seq2.cpp -o seq -lssl -lcrypto -fpermissive
  ./seq | ./seq `key`
```
```
  g++ seq3.cpp -o seq -lssl -lcrypto -fpermissive
  ./seq | ./seq `key`
```