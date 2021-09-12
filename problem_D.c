//PROBLEM d: The Sleeping Barbers Problem
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define EMPTY_VAL -1
#define SEATS_AMOUNT 5
#define CUSTOMERS_AMOUNT 300
#define RANDOM_MAX 700000 //random ms will be between 0ms and 0.7ms

int seats[SEATS_AMOUNT] = {EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL, EMPTY_VAL};
bool customer = false;
bool keepGoing = true;

pthread_cond_t barber_sleep;
pthread_cond_t customer_wait;

pthread_mutex_t seat_lock;

void * barber(void * param) {
   //seed random generator in each thread
   srand(time(NULL));
   while(keepGoing) {
      printf("***barber, locking mutex8**\n");
      pthread_mutex_lock(&seat_lock); 
      {
         printf("----barber, checking if there are any customers (enter cond_wait)\n");
         //wait for a customer to come in
         while(!customer) {
            pthread_cond_wait(&barber_sleep, &seat_lock);
         }
         printf("--------barber waking up (cond_signal received)\n");
         //while the first seat has a customer
         while(seats[0]!=EMPTY_VAL) {
               //serve the customer
               printf("--------barber servicing customer %d\n", seats[0]);
               int randomNo = rand() % RANDOM_MAX;
               usleep(randomNo);
               //remove them from the seat and move other customers forward in queue (dequeue)
               printf("------------barber removing customer %d from seats array\n", seats[0]);
               for(int i=0; i<SEATS_AMOUNT-1; i++) {
                  seats[i] = seats[i+1];
               }
               //last seat empty (leftover)
               seats[SEATS_AMOUNT-1] = EMPTY_VAL;
         }
         customer = false;
      }
      printf("***barber, unlocking mutex***\n");
      pthread_mutex_unlock(&seat_lock);

   }
   return NULL;
}

void * customerThread(void * param) {
      //seed random generator in each thread
      srand(time(NULL));
      long customerNo = (long) param;
      int mySeat = EMPTY_VAL;
      printf("***customer %ld lock mutex***\n", customerNo);
      pthread_mutex_lock(&seat_lock); 
      {
         // check if a seat is available
         for(int i=0;i<SEATS_AMOUNT;i++) {
            //occupy first empty seat (no seat already found)
            if(seats[i]==EMPTY_VAL&&mySeat==EMPTY_VAL) {
               mySeat = i;
               printf("----seat %d is available! customer %ld sitting in it (adding to seats array)\n", i, customerNo);
               seats[i] = customerNo;
            }
         }
         if(mySeat!=EMPTY_VAL) {
            //tell the barber to wake up and serve me
            customer = true;
            printf("--------telling barber to wake up with cond_signal\n");
            pthread_cond_signal(&barber_sleep);
            printf("***customer %ld unlock mutex***\n", customerNo);
            pthread_mutex_unlock(&seat_lock);
         }
         else {
            printf("----there's no free seats, customer %ld is leaving (unlock mutex)\n", customerNo);
            pthread_mutex_unlock(&seat_lock);
            pthread_cond_signal(&barber_sleep);
         }
      return NULL;
   }
}

void * generateCustomers(void * param) {
   //seed random generator in each thread
   srand(time(NULL));
   //large array of customers to keep customers coming in for whole 10 secs
   pthread_t tid [CUSTOMERS_AMOUNT];
   int i = 0;
   while(keepGoing && i<CUSTOMERS_AMOUNT) {
      int randomNo = rand() % RANDOM_MAX;
      usleep(randomNo);
      pthread_create(&tid[i], NULL, customerThread, (void *) (long) i);
      i++;
   }
   pthread_exit(NULL);
}


int main(void) {
   //seed random generator in each thread
   srand(time(NULL));
   pthread_mutex_init(&seat_lock, NULL);
   pthread_cond_init(&barber_sleep, NULL);
   pthread_t barber_id;
   pthread_t enter_customers_id;

   int result;
   int counter = 0;
   result = pthread_create(&barber_id, NULL, barber, NULL);
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
}