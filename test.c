#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// this programs opens the input myInput.txt, calculates the prefix sum serially and compares it to out.txt produced by the parallel implementation7 

int main(){
  int *a;
  int n, i;

  FILE *fid = fopen ("myInput.txt" ,"r");
  fscanf (fid, "%d ", &n);

  a = malloc( n * sizeof(int));
  for (i = 0; i < n; i++){
    fscanf (fid, "%d", &a[i]);
  }

  for (i = 1; i < n; i++){
    a[i] += a[i-1];
  }

  FILE *fid1 = fopen ("out.txt" ,"r");

  int x;
  int inid = 1;
  for (i = 0; i < n; i++){
    fscanf (fid1, "%d", &x);
    if (x != a[i]){
      inid = 0;
    }
  }

  printf("the size is %d and result is %d\n", n, inid);

  return 0;
}
