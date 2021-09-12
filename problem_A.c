//PROBLEM A: The Producer-Consumer Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define EMPTY_VAL -1
#define BUCKETS_AMOUNT 5
#define RANDOM_MAX 1000

int buckets[BUCKETS_AMOUNT] = {EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL};
//int buffer = EMPTY_VAL;
bool full = false;
bool keepGoing = true;

pthread_cond_t wait_here;

pthread_mutex_t buckets_lock;

void * producer(void * param) {
   while(keepGoing) {
      pthread_mutex_lock(&buckets_lock); 
      {
         //printf("producer, checking if empty\n");
         while(full) {
            pthread_cond_wait(&wait_here, &buckets_lock);
         }
         for(int i=0; i<BUCKETS_AMOUNT; i++) {
            if(buckets[i]==EMPTY_VAL) {
               int randomNo = rand() % RANDOM_MAX;
               buckets[i] = randomNo;
            }
         }
         //full if there is at least one number in the bucket
         ///not full if there is at least one empty bucket
         full = true;
         //sleep(1);
      }
      pthread_mutex_unlock(&buckets_lock);
      pthread_cond_broadcast(&wait_here);
      //printf("loop end\n");
   }
   //printf("thread exiting\n");
   exit(EXIT_SUCCESS);
}

void * consumer(void * param) {
   while(keepGoing) {
      pthread_mutex_lock(&buckets_lock); 
      {
         //printf("consumer, checking if full\n");
         while(!full) {
            pthread_cond_wait(&wait_here, &buckets_lock);
         }
         for(int i=0; i<BUCKETS_AMOUNT; i++) {
            if(buckets[i]!=EMPTY_VAL) {
               printf("no. is %d\n", buckets[i]);
               buckets[i] = EMPTY_VAL;
            }
         }
         full = false;
         //sleep(1);
      }
      pthread_mutex_unlock(&buckets_lock);
      pthread_cond_broadcast(&wait_here);
      //printf("loop end\n");
   }
   //printf("thread exiting\n");
   exit(EXIT_SUCCESS);
}

void * generateThreads(void * param) {
   pthread_t tid [10];
   //int i = 0;
   for(int i=0; i<10; i++) {
      if(i%2==0) {
         //printf("making p thread\n");
         pthread_create(&tid[i], NULL, producer, (void *) (long) i);
      }
      else {
         //printf("making c thread\n");
         pthread_create(&tid[i], NULL, consumer, (void *) (long) i);
      }
   }
   pthread_exit(NULL);
}


int main(void) {
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
