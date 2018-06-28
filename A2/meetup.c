/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- STARTS here.
 */
static int group_size;
static int meet_val;
static resource_t codeword;

static sem_t sem_single;
static sem_t sem_group;
static sem_t sem_leader;
static int count;

void initialize_meetup(int n, int mf) {
    
    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initializes the shared structures, including those used for
     * synchronization.
     */
    group_size = n;
    meet_val = mf;
    init_resource(&codeword, "meetup");

    sem_init(&sem_single, 0, 1);
    sem_init(&sem_group, 0, 0);
    sem_init(&sem_leader, 0, 0);
    count = 0;
}


void join_meetup(char *value, int len) {
    int length;
    if(len>MAX_VALUE_LEN){
        length=MAX_VALUE_LEN;
    } else {
        length = len;
    }
    sem_wait(&sem_single);
    count++;
    if (count <group_size) {
        // if meet first, record first value
        if((meet_val) && (count == 1)){
            write_resource(&codeword, value, length);
        }
        sem_post(&sem_single);
        // Waits until last thread for the group arrives
        sem_wait(&sem_group);
        read_resource(&codeword, value, MAX_VALUE_LEN);
        count--;
        //Ensures all threads in current group have read before
        //next group starts reading and writing.
        if(count == 1){
            sem_post(&sem_leader);
        } else {
            sem_post(&sem_group);
        }
    } else {
        if(group_size > 1){
            //checks for meetlast
            if(!meet_val) {
                write_resource(&codeword, value, length);
            }
            read_resource(&codeword, value, MAX_VALUE_LEN);
            sem_post(&sem_group);
            sem_wait(&sem_leader);
        } else {
            write_resource(&codeword, value, length);
        }
        count--;
        sem_post(&sem_single);
    }
}
