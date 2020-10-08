#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

using namespace std;
int thread_count = 2;
int m = 8000;
int n = 8000;

double* A;
double* x;
double* y;

void *Pth_mat_vect(void* rank) {
   long my_rank = (long) rank;
   int i, j;
   int local_m = m/thread_count;
   int my_first_row = my_rank*local_m;
   int my_last_row = (my_rank+1)*local_m - 1;

   for (i = my_first_row; i <= my_last_row; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
          y[i] += A[i*n+j]*x[j];
   }

   return NULL;
}

int main(int argc, char* argv[]) {
   srand (time(NULL));
   long thread;
   pthread_t* ids = (pthread_t *)malloc(thread_count*sizeof(pthread_t));

   A = (double *)malloc(m*n*sizeof(double));
   x = (double *)malloc(n*sizeof(double));
   y = (double *)malloc(m*sizeof(double));


   int i, j;
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = rand() % 10 + 1;

   for (i = 0; i < n; i++)
      x[i] = rand() % 10 + 1;

   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&ids[thread], NULL,Pth_mat_vect, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(ids[thread], NULL);

   free(A);
   free(x);
   free(y);

   return 0;
}
