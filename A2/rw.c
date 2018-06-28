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
 * variables -- must START here.
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
     * Initialize the shared structures, including those used for
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
    while (!(writers == 0)) {
        pthread_cond_wait(&readersQ, &m);
    }
    readers++;
    pthread_mutex_unlock(&m);
    read_resource(&data, value, len);

    pthread_mutex_lock(&m);
    if (--readers == 0) {
        pthread_cond_signal(&writersQ);
    }
    pthread_mutex_unlock(&m);
}


void rw_write(char *value, int len) {
    pthread_mutex_lock(&m);
    writers++;
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
    if (writers) {
        pthread_cond_signal(&writersQ);
    } else {
        pthread_cond_broadcast(&readersQ);
    }
    pthread_mutex_unlock(&m);
}
