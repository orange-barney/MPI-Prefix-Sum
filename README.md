# MPI-Prefix-Sum
a parallel implementation for calculating the prefix sum of an array using MPI.
the code is commented and easy to interpret

files:
code.c contains the code
test.c to test the the parallel implementaion result
myInput.txt has a randomly generated array of size 2^20 with values between 1 and 5 inclusive

compile: mpicc code code.c
run: qsubmpi code numOfCores inputFile.txt

compile test.c: gcc -o test test.c
