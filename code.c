#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//created to return log base 2 of a number // instead of using math library
int logb2 (int x){
  if (x == 1){
    return 0;
  }
  else if (x == 2){
      return 1;
  }
  else if (x == 4){
      return 2;
  }
  else if (x == 8){
      return 3;
  }
  else if (x == 16){
      return 4;
  }
  else if (x == 32){
        return 5;
  }
  return 0;
}

int main(int argc, char** argv){
    int rank, size, i;
    char version [MPI_MAX_LIBRARY_VERSION_STRING];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n; // for size of input array
    int *arr; // for input array

    if (rank == 0){
      //reading from input file to the array
      FILE *fid = fopen (argv[1] ,"r");
    	fscanf (fid, "%d ", &n);

      arr = malloc(n * sizeof(int));
      for (i = 0; i < n; i++){
        fscanf (fid, "%d", &arr[i]);
      }
      ///////////

      //sending array size to slaves
      for (i = 1; i < size; i++){
        MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
      ////////////
    }
    ////slaves receiving array size n
    if (rank != 0){
      MPI_Recv (&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    //////////

    int localSize = n / size; //calculting local size of array of the procs

    int* localArray;
    localArray = malloc(localSize * sizeof(int));;

    //scatter the original array among all the procs. scatter array 'arr' (dividing it with size localSize), and fill the localArray
    MPI_Scatter(arr, localSize, MPI_INT, localArray, localSize, MPI_INT, 0, MPI_COMM_WORLD);

    //calculting prefix sum serially for every procs
    for (i = 1; i < localSize; i++){
      localArray[i] += localArray[i - 1];
    }

    int logP = logb2(size);

    //stride is initialized to 1 and every loop it is multiplied by 2
    int stride = 1;
    //printf("logP is: %d\n",logP);
    for (i = 0; i < logP; i++){ //it will do log(p) iterations
      int sum = localArray[localSize - 1]; // the current sum of the local array
      int receivedSum = 0; //to store the sum received from other procs

      if (rank + stride <= size - 1){ // if it can send its sum to another proc
        MPI_Send(&sum, 1, MPI_INT, rank + stride, 0, MPI_COMM_WORLD);
        //printf("loop: %d transfer. sending from %d to %d\n",i,rank, rank + stride );
      }
      if (rank - stride >= 0){// if it can receive a sum from aother proc
        MPI_Recv (&receivedSum, 1, MPI_INT, rank - stride, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //printf("loop: %d transfer. receiving from %d to %d\n",i, rank - stride, rank);
      }

      ///add the recieved sum to every element in the local array
      int j;
      for (j = 0; j < localSize; j++){
        localArray[j] += receivedSum;
      }
      stride *= 2;

      // barrier to wait all proc to reach to this level then they all proceed.
      int res;
      res = MPI_Barrier(MPI_COMM_WORLD);
      if(res != MPI_SUCCESS){
        //printf("error\n");
        exit(0);
      }
    }

    //gather the local arrays form the procs and gather them in arr
    MPI_Gather(localArray, localSize, MPI_INT, arr, localSize, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0){
      //print output
      FILE *fout = fopen ("out.txt" ,"w+");
      for (i = 0; i < n; i++){
        fprintf(fout, "%d ", arr[i]);
      }
    }
    MPI_Finalize();
    return 0;
}
