/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must STARTS here.
 */

static resource_t data;
static int readers;
static int writers;
static int active_writers;
static pthread_mutex_t m;
static pthread_cond_t readersQ;
static pthread_cond_t writersQ;
static char val[MAX_VALUE_LEN];


void initialize_readers_writer() {
    /*
     * Initializes the shared structures, including those used for
     * synchronization.
     */
    init_resource(&data, "rw");
    readers = 0;
    writers = 0;
    active_writers =  0;
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&readersQ, NULL);
    pthread_cond_init(&writersQ, NULL);
}


void rw_read(char *value, int len) {
    pthread_mutex_lock(&m);
    //Will wait for any thread that is writing or wants to write
    while (!(writers == 0)) {
        pthread_cond_wait(&readersQ, &m);
    }
    readers++;
    // writers are prevented from writing since readers > 0 at this point and
    // mutex can be unlocked to allow in more readers.
    pthread_mutex_unlock(&m);
    read_resource(&data, value, len);

    // The lock here prevents the signal in case
    // there is a reader in it's critical section before the 
    // readers++ statement is executedm since that might cause
    // --readers == 0 even though a reader is about to read.
    pthread_mutex_lock(&m);
    if (--readers == 0) {
        pthread_cond_signal(&writersQ);
    }
    pthread_mutex_unlock(&m);
}


void rw_write(char *value, int len) {
    pthread_mutex_lock(&m);
    writers++;
    // Only allows a thread to write if there are currently no
    // readers and writers.
    while(!((readers == 0) && (active_writers == 0))) {
        pthread_cond_wait(&writersQ, &m);
    }

    active_writers++;
    pthread_mutex_unlock(&m);
    
    strncpy(val,value,MAX_VALUE_LEN);
    write_resource(&data,val,len);
    
    pthread_mutex_lock(&m);
    writers--;
    active_writers--;
    // Ensures writers priority by signaling a waiting writer if
    // there exists one. Otherwise broadcasts readers.
    if (writers) {
        pthread_cond_signal(&writersQ);
    } else {
        pthread_cond_broadcast(&readersQ);
    }
    pthread_mutex_unlock(&m);
}
