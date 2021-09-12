//PROBLEM d: The Producer-Consumer Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define EMPTY_VAL -1
#define SEATS_AMOUNT 5
#define CUSTOMERS_AMOUNT 200
#define RANDOM_MAX 700000

//if buffer = 0, there is no customer. Otherwise, there is a customer
int seats[SEATS_AMOUNT] = {EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL};
bool customer = false;
bool keepGoing = true;

pthread_cond_t barber_sleep;
pthread_cond_t customer_wait;

pthread_mutex_t seat_lock;

void * producer(void * param) {
   while(keepGoing) {
      pthread_mutex_lock(&seat_lock); 
      {
         printf("barber, checking if there are any customers (enter cond_wait)\n");
         while(!customer) {
            printf("----barber looping)\n");
            pthread_cond_wait(&barber_sleep, &seat_lock);
         }
         printf("----barber waking up (cond_signal received)\n");
         //go through all seats and check for customers
         //while the first seat has a customer
         while(seats[0]!=EMPTY_VAL) {
               //serve the customer
               printf("--------barber servicing customer %d in first seat\n", seats[0]);
               int randomNo = rand() % RANDOM_MAX;
               usleep(randomNo);
               //remove them from the seat
               //shuffle each other customer
               printf("------------barber removing customer %d from seats array\n", seats[0]);
               for(int i=0; i<SEATS_AMOUNT-1; i++) {
                  seats[i] = seats[i+1];
               }
               //new empty seat is EMPTY_VAL
               seats[SEATS_AMOUNT-1] = EMPTY_VAL;
         }
         customer = false;
          printf(customer ? "true" : "false");
      }
      pthread_mutex_unlock(&seat_lock);
      pthread_cond_signal(&barber_sleep);
   }
   pthread_cond_signal(&barber_sleep);
   pthread_exit(NULL);
}

void * consumer(void * param) {
      long customerNo = (long) param;
      bool seatAvailable = false;
      int mySeat = EMPTY_VAL;
      pthread_mutex_lock(&seat_lock); 
      {
         //customer enters the store
         //printf("customer %ld enter store, check seat\n", customerNo);
         // check if a seat is available
         for(int i=0;i<4;i++) {
            if(seats[i]==EMPTY_VAL&&mySeat==EMPTY_VAL) {
               mySeat = i;
               printf("seat %d is available! customer %ld sitting in it (adding to seats array)\n", i, customerNo);
               //sleep(1);
               seats[i] = customerNo;

            }
         }
         if(mySeat!=EMPTY_VAL) {
            //tell the barber to wake up and serve me
            customer = true;
            //pthread_mutex_unlock(&seat_lock);
            printf("----telling barber to wake up with cond_signal\n");
            pthread_cond_signal(&barber_sleep);
            while(customer) {
               pthread_cond_wait(&barber_sleep, &seat_lock);
            }
            printf("--------customer %ld leaving after haircut\n", customerNo);
            //printf("no. is %d\n", seat);
            //sleep(1);
            pthread_mutex_unlock(&seat_lock);
            //pthread_cond_signal(&barber_sleep);
            
         }
         // wait for barber to finish
         else {
            printf("there's no free seats, customer %ld is leaving (unlock mutex)\n", customerNo);
            //sleep(1);
            pthread_mutex_unlock(&seat_lock);
            pthread_cond_signal(&barber_sleep);
            printf(customer ? "true" : "false");
            
            //exit(EXIT_SUCCESS);
         }
      pthread_exit(NULL);
   }
}

void * generateCustomers(void * param) {
   pthread_t tid [CUSTOMERS_AMOUNT];
   int i = 0;
   while(keepGoing && i<CUSTOMERS_AMOUNT) {
      int randomNo = rand() % RANDOM_MAX;
      usleep(randomNo);
      pthread_create(&tid[i], NULL, consumer, (void *) (long) i);
      i++;
   }
   pthread_exit(NULL);
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
   keepGoing = false;
   pthread_exit(NULL);
   //return EXIT_SUCCESS;
}