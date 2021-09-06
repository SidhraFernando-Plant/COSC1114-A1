//PROBLEM A: The Producer-Consumer Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

//if buffer = 0, there is no customer. Otherwise, there is a customer
int seat = -1;
bool customer = false;
bool barberWorking = false;

pthread_cond_t barber_sleep;
pthread_cond_t customer_wait;

pthread_mutex_t seat_lock;

void * producer(void * param) {
   while(true) {
      pthread_mutex_lock(&seat_lock); 
      {
         printf("barber, checking if seat is empty\n");
         while(!customer) {
            pthread_cond_wait(&barber_sleep, &seat_lock);
         }
         printf("----barber, waking up\n");
         //service a customer
         printf("--------barber servicing customer\n");
         printf("------------barber removing customer from seat\n");
         seat = -1;
         customer = false;

         sleep(1);
      }
      pthread_mutex_unlock(&seat_lock);
      pthread_cond_signal(&barber_sleep);
   }
}

void * consumer(void * param) {
      long i = (long) param;
      pthread_mutex_lock(&seat_lock); 
      {
         //customer enters the store
         printf("customer %ld enter store, check seat\n", i);
         //if a seatt is available
         if(seat==-1) {
            printf("----a seat is available! sitting in it\n");
            seat = 5;
            //tell the barber to wake up and serve me
            customer = true;
            pthread_mutex_unlock(&seat_lock);
            pthread_cond_signal(&barber_sleep);
            while(customer) {
               pthread_cond_wait(&barber_sleep, &seat_lock);
            }
            printf("--------customer %ld leaving after haircut\n", i);
            printf("no. is %d\n", seat);
            sleep(1);
            pthread_mutex_unlock(&seat_lock);
            pthread_cond_signal(&barber_sleep);
         }
         // wait for barber to finish
         else {
            printf("----leaving bc no seatts\n");
            pthread_mutex_unlock(&seat_lock);
            sleep(1);
            //exit(EXIT_SUCCESS);
         }
      
      
   }
}


int main(void) {
   pthread_mutex_init(&seat_lock, NULL);
   pthread_cond_init(&barber_sleep, NULL);
   pthread_t barber_id;
   //pthread_t cons_id;

   int result;
   result = pthread_create(&barber_id, NULL, producer, NULL);

   if (result) {
      return EXIT_FAILURE;
   }
   
   pthread_t tid [4];
   for(int i=0; i<4; i++) {
       pthread_create(&tid[i], NULL, consumer, (void *) (long) i);
   }


   sleep(10);
   exit(EXIT_SUCCESS);
   //return EXIT_SUCCESS;
}