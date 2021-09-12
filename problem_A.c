//PROBLEM A: The Producer-Consumer Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int buckets[5] = {-1, -1, -1, -1, -1};
//int buffer = -1;
bool full = false;
bool keepGoing = true;

pthread_cond_t wait_here;

pthread_mutex_t buckets_lock;

const char LAST_CHAR = 'F';

void * producer(void * param) {
   while(keepGoing) {
      pthread_mutex_lock(&buckets_lock); 
      {
         //printf("producer, checking if empty\n");
         while(full) {
            pthread_cond_wait(&wait_here, &buckets_lock);
         }
         for(int i=0; i<5; i++) {
            if(buckets[i]==-1) {
               int randomNo = rand() % 1000;
               buckets[i] = randomNo;
            }
         }
         //full if there is at least one number in the bucket
         ///not full if there is at least one empty bucket
         full = true;
         //sleep(1);
      }
      pthread_mutex_unlock(&buckets_lock);
      pthread_cond_signal(&wait_here);
   }
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
         for(int i=0; i<5; i++) {
            if(buckets[i]!=-1) {
               printf("no. is %d\n", buckets[i]);
               buckets[i] = -1;
            }
         }
         full = false;
         //sleep(1);
      }
      pthread_mutex_unlock(&buckets_lock);
      pthread_cond_signal(&wait_here);
   }
   exit(EXIT_SUCCESS);
}


int main(void) {
   pthread_mutex_init(&buckets_lock, NULL);
   pthread_cond_init(&wait_here, NULL);
   pthread_t prod_id;
   pthread_t cons_id;

   int result;
   result = pthread_create(&prod_id, NULL, producer, NULL);

   if (result) {
      return EXIT_FAILURE;
   }
   
   result = pthread_create(&cons_id, NULL, consumer, NULL);

   if (result) {
      return EXIT_FAILURE;
   }

   sleep(10);
   keepGoing = false;
   //exit(EXIT_SUCCESS);
   pthread_exit(NULL);
}
