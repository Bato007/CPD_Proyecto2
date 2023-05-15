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