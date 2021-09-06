//PROBLEM A: The Producer-Consumer Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int buffer = -1;
bool full = false;

pthread_cond_t wait_here;

pthread_mutex_t buffer_lock;

const char LAST_CHAR = 'F';

void * producer(void * param) {
   while(true) {
      pthread_mutex_lock(&buffer_lock); 
      {
         //printf("producer, checking if empty\n");
         while(full) {
            pthread_cond_wait(&wait_here, &buffer_lock);
         }
         int randomNo = rand() % 1000;
         buffer = randomNo;
         full = true;
         sleep(1);
      }
      pthread_mutex_unlock(&buffer_lock);
      pthread_cond_signal(&wait_here);
   }
}

void * consumer(void * param) {
   while(true) {
      pthread_mutex_lock(&buffer_lock); 
      {
         //printf("consumer, checking if full\n");
         while(!full) {
            pthread_cond_wait(&wait_here, &buffer_lock);
         }
         printf("no. is %d\n", buffer);
         full = false;
         buffer = -1;
         sleep(1);
      }
      pthread_mutex_unlock(&buffer_lock);
      pthread_cond_signal(&wait_here);
   }
}


int main(void) {
   pthread_mutex_init(&buffer_lock, NULL);
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
   exit(EXIT_SUCCESS);
   //return EXIT_SUCCESS;
}