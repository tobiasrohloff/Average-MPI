# Average with MPI

Implement a parallel MPI program that computes the integer-precision average and the double- precision average of a given set of double values at the same time.

The integer-precision average are to be calculated based on the integer versions of the input values. They are to be computed by reading the double values from an input file, converting them with floor() and casting them to int. The double-precision average can be computed directly using the input values.

Your program is only allowed to use collective MPI operations for the coordination of the parallel computation. MPI_Send and MPI_Receive (and their variations) are disallowed.

## Input

Your application has to be named „mpiavg“ and hast to accept three parameters:
- The file name of the data file that contains the input numbers.
- The number of MPI ranks to be used for the integer-precision average computation.
- The number of MPI ranks to be used for the double-precision average computation.

The data file is in the current working directory of the program. It contains one double value per line (“4.84637”).

We will run your application as follows, with variations in the numerical parameters:

`mpirun --cpus-per-proc 2 -np 16 mpiavg data.txt 7 9`

Example content of “data.txt”:
```
5.666
4.3234
7.3434
2.434 1.0
```

## Output

The program must terminate with exit code 0 and has to produce an output file with the name “output.txt” in the same directory. That file has to contain two double numbers: first the integer- precision average, and then the float-precision average.

Example content of “output.txt”:
```
3.800000
4.153360
```
