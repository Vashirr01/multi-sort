#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "cputime.h"

void merge(int *arr, int n, int mid) {
  int left = 0, right = mid, i;
  int *x = malloc(n * sizeof(int));
  // copy the ith item from either the left or right part
  for (i = 0; i < n; i++) {
    if (right == n)
      x[i] = arr[left++];
    else if (left == mid)
      x[i] = arr[right++];
    else if (arr[right] < arr[left])
      x[i] = arr[right++];
    else
      x[i] = arr[left++];
  }
  // transfer from temporary array back to given one
  for (i = 0; i < n; i++)
    arr[i] = x[i];
  free(x);
}

struct msortargs {
  int *arr;
  int n;
};


void msort(int *arr, int n); 

void *left_entry(void* args) {
  int *arr = ((struct msortargs*)args)->arr;
  int n = ((struct msortargs*)args)->n;
  msort(arr, n);
  return NULL;
}

void msort(int *arr, int n) {
  if (n < 2)
    return;
  int mid = n / 2;
  msort(arr, mid);
  msort(arr+mid, n-mid);
  merge(arr, n, mid);
}

void msort_divide(int *arr, int n) {
  pthread_t p1; 
  int mid = n / 2;
  struct msortargs args = {arr, mid};
  pthread_create(&p1, NULL, left_entry, (void*)&args);
  msort(arr+mid, n-mid);
  pthread_join(p1, NULL);
  merge(arr, n, mid);
}


int main () {
  int n = 1 << 24;
  int *arr = malloc(n * sizeof(int));
  // populate array with n many random integers
  srand(1234);
  for (int i = 0; i < n; i++)
    arr[i] = rand();

  fprintf(stderr, "Sorting %d random integers\n", n);

  // actually sort, and time cpu use
  struct profile_times t;
  profile_start(&t);
  msort_divide(arr, n);
  profile_log(&t);

  // assert that the output is sorted
  for (int i = 0; i < n - 1; i++)
    if (arr[i] > arr[i + 1]) {
      printf("error: arr[%d] = %d > arr[%d] = %d\n", i, arr[i], i + 1,
             arr[i + 1]);
      return 1;
    }
  return 0;
}
