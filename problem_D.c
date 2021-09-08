//PROBLEM d: The Producer-Consumer Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

//if buffer = 0, there is no customer. Otherwise, there is a customer
int seats[5] = {-1, -1, -1, -1, -1};
bool customer = false;
bool barberWorking = false;
bool keepGoing = true;

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
         //go through all seats and check for customers
         for(int i=0;i<5;i++) {
            //if a customer is in the seat
            if(seats[i]!=-1) {
               //serve the customer
               printf("--------barber servicing customer %d\n", seats[i]);
               //int randomNo = rand() % 10000;
               //remove them from the seat
               printf("------------barber removing customer from seat %d\n", i);
               seats[i] = -1;
               
            }
         }
         customer = false;
      }
      pthread_mutex_unlock(&seat_lock);
      pthread_cond_signal(&barber_sleep);
   }
}

void * consumer(void * param) {
      long customerNo = (long) param;
      bool seatAvailable = false;
      int mySeat = -1;
      pthread_mutex_lock(&seat_lock); 
      {
         //customer enters the store
         //printf("customer %ld enter store, check seat\n", customerNo);
         // check if a seat is available
         for(int i=0;i<5;i++) {
            if(seats[i]==-1&&mySeat==-1) {
               mySeat = i;
               printf("----seat %d is available! sitting in it\n", i);
               //sleep(1);
               seats[i] = customerNo;

            }
         }
         printf("my seat is %d \n", mySeat);
         if(mySeat!=-1) {
            //tell the barber to wake up and serve me
            customer = true;
            printf("Waking up barber\n");
            //pthread_mutex_unlock(&seat_lock);
            pthread_cond_signal(&barber_sleep);
            printf("waiting for barber\n");
            while(customer) {
               pthread_cond_wait(&barber_sleep, &seat_lock);
            }
            printf("--------customer %ld leaving after haircut\n", customerNo);
            //printf("no. is %d\n", seat);
            //sleep(1);
            pthread_mutex_unlock(&seat_lock);
            pthread_cond_signal(&barber_sleep);
            
         }
         // wait for barber to finish
         else {
            //printf("----leaving bc no seatts\n");
            //sleep(1);
            pthread_mutex_unlock(&seat_lock);
            
            //exit(EXIT_SUCCESS);
         }
   }
}

void * generateCustomers(void * param) {
   printf("customers thread createtd\n");
      pthread_t tid [100];
      for(int i=0; i<100; i++) {
         pthread_create(&tid[i], NULL, consumer, (void *) (long) i);
      }
   
}


int main(void) {
   pthread_mutex_init(&seat_lock, NULL);
   pthread_cond_init(&barber_sleep, NULL);
   pthread_t barber_id;
   pthread_t enter_customers_id;
   //pthread_t cons_id;

   int result;
   int counter = 0;
   result = pthread_create(&barber_id, NULL, producer, NULL);

   if (result) {
      return EXIT_FAILURE;
   }

   result = pthread_create(&enter_customers_id, NULL, generateCustomers, NULL);

   if (result) {
      return EXIT_FAILURE;
   }

   sleep(10);
   exit(EXIT_SUCCESS);
   //return EXIT_SUCCESS;
}