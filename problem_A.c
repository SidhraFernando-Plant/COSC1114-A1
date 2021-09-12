//PROBLEM A: The Producer-Consumer Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define EMPTY_VAL -1
#define BUCKETS_AMOUNT 5
#define PRODUCERS_AMOUNT 5
#define CONSUMERS_AMOUNT 5
#define RANDOM_MAX 1000

int buckets[BUCKETS_AMOUNT] = {EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL};
bool full = false;
bool keepGoing = true;

pthread_cond_t wait_here;

pthread_mutex_t buckets_lock;
     

void * producer(void * param) {
   //seed random generator in each thread
   srand(time(NULL));
   while(keepGoing) {
      printf("a producer locking buckets  ||  ");
      pthread_mutex_lock(&buckets_lock); 
      {
         printf("a producer checking if buckets empty  ||  ");
         while(full) {
            pthread_cond_wait(&wait_here, &buckets_lock);
         }
         
         for(int i=0; i<BUCKETS_AMOUNT; i++) {
            
            //if a bucket is empty, fill it
            if(buckets[i]==EMPTY_VAL) {
               int randomNo = rand() % RANDOM_MAX;
               buckets[i] = randomNo;
            }
         }
         printf("a producer filled buckets array  ||  ");
         //buckets are now full, update variable
         full = true;
         sleep(1);
      }
      printf("a producer unlocking buckets  ||  ");
      pthread_mutex_unlock(&buckets_lock);
      pthread_cond_broadcast(&wait_here);
   }
   return NULL;
}

void * consumer(void * param) {
   //seed random generator in each thread
   srand(time(NULL));
   while(keepGoing) {
      printf("a consumer locking buckets  ||  ");
      pthread_mutex_lock(&buckets_lock); 
      {
         printf("a consumer checking if buckets full  ||  ");
         while(!full) {
            pthread_cond_wait(&wait_here, &buckets_lock);
         }
         for(int i=0; i<BUCKETS_AMOUNT; i++) {
            if(buckets[i]!=EMPTY_VAL) {
               printf("\nno. is %d", buckets[i]);
               buckets[i] = EMPTY_VAL;
            }
         }
         full = false;
         sleep(1);
      }
      printf("a consumer unlocking buckets  ||  ");
      pthread_mutex_unlock(&buckets_lock);
      pthread_cond_broadcast(&wait_here);
      //printf("loop end\n");
   }
   //printf("thread exiting\n");
   return NULL;
}

void * generateThreads(void * param) {
   //seed random generator in each thread
   srand(time(NULL));
   int totalThreads = PRODUCERS_AMOUNT + CONSUMERS_AMOUNT;
   pthread_t tid [totalThreads];
   //int i = 0;
   for(int i=0; i<totalThreads; i++) {
      //this will work in cases where we want to make an equal amount of producers and consumers
      if(i%2==0) {
         pthread_create(&tid[i], NULL, producer, NULL);
      }
      else {
         //make every second thread a consumer, other threads will be producers
         pthread_create(&tid[i], NULL, consumer, NULL);
      }
   }
   pthread_exit(NULL);
}


int main(void) {
   //seed random generator in each thread
   srand(time(NULL));
   pthread_mutex_init(&buckets_lock, NULL);
   pthread_cond_init(&wait_here, NULL);
   pthread_t prod_id;
   pthread_t cons_id;
   pthread_t enter_customers_id;

   int result;

   result = pthread_create(&enter_customers_id, NULL, generateThreads, NULL);

   if (result) {
      return EXIT_FAILURE;
   }

   sleep(10);
   keepGoing = false;
   //exit(EXIT_SUCCESS);
   pthread_exit(NULL);
}
