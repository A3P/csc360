#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;

void print_task(taskval_t *t, void *arg) {
    printf("task %03d: %5d %3.2f %3.2f\n",
        t->id,
        t->arrival_time,
        t->cpu_request,
        t->cpu_used
    );  
}


void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}

// if an event arrives at the passed in tick, then remove it from the
// event list and push it to the queue provided in the arguements
taskval_t *check_arrival(taskval_t *t, int tick){
    if(event_list != NULL && event_list->arrival_time <= tick){
        taskval_t* temp = event_list;
        event_list = remove_front(event_list);
        //delete
        printf("arrived at %05d: ",tick);
        print_task(temp,NULL);

        t = add_end(t, temp);
    }
    // Have to return t in case queue was NULL when the 
    // function was called
    return t;
}

void run_simulation(int qlen, int dlen) {
    int tick = 0;

    taskval_t *queue = NULL;

    //delete
    taskval_t* node = event_list;
    printf("event list: \n");
    while(node){
        print_task(node,NULL);
        if(node->cpu_request == 2){
            printf("it's equal");
        }
        node = remove_front(node);
    }
    
    // Runs the simulation until no more incoming, or ready events are left
    while(event_list != NULL || queue != NULL){
        queue = check_arrival(queue, tick);

        // Will process task from the front of queue if not empty or else
        // will "IDLE"
        if(queue != NULL){
            int i;
            // Simulates the dispatch
            for( i = 0; i<dlen; i++){
                printf("[%05d] DISPATCHING\n",tick++);
                queue = check_arrival(queue, tick);
            }
            // Simulates the ticking of the tasks
            for( i = 0; i<qlen; i++){
                printf(
                    "[%05d] id=%05d req=%.2f used=%.2f\n",
                    tick++, queue->id, queue->cpu_request, queue->cpu_used
                );
                printf("It gets here\n");
                queue = check_arrival(queue, tick);
                
                //delete
                if(queue->next != NULL){
                    printf("2nd task in queue: ");
                    print_task(queue->next,NULL);
                }

                queue->cpu_used += 1;
                // Check if enough ticks are used for the task and if 
                // finished it prints the relevant data
                if(queue->cpu_request <= (int)queue->cpu_used){
                    queue->cpu_used = queue->cpu_request;
                    printf("time before assignment: %d\n",queue->finish_time);
                    queue->finish_time = tick;
                    float turn_around = queue->finish_time - queue->arrival_time;
                    printf(
                        "[%05d] id=%05d EXIT w=%.2f ta=%.2f\n",
                        tick, queue->id, 
                        turn_around - queue->cpu_used, turn_around
                    );
                    break;
                }
            }
            // Remove task from queue. De-allocates the task's memory if
            // the task is done or else it's moved to the back of the queue  
            taskval_t* temp = queue;
            queue = remove_front(queue);
            if(temp->finish_time) {
                printf("Not adding task to end\n");
                end_task(temp);
            } else {
                // Have to reassign queue in case there is only one task else
                // it will be equal to NULL after remove_front function.
                queue = add_end(queue, temp);
            }
        } else {
            printf("[%05d] IDLE\n",tick++);
        }

    }
    printf("end of sim\n");
}


int main(int argc, char *argv[]) {
    char   input_line[MAX_BUFFER_LEN];
    int    i;
    int    task_num;
    int    task_arrival;
    float  task_cpu;
    int    quantum_length = -1;
    int    dispatch_length = -1;

    taskval_t *temp_task;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quantum") == 0 && i+1 < argc) {
            quantum_length = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--dispatch") == 0 && i+1 < argc) {
            dispatch_length = atoi(argv[i+1]);
        }
    }

    if (quantum_length == -1 || dispatch_length == -1) {
        fprintf(stderr, 
            "usage: %s --quantum <num> --dispatch <num>\n",
            argv[0]);
        exit(1);
    }


    while(fgets(input_line, MAX_BUFFER_LEN, stdin)) {
        sscanf(input_line, "%d %d %f", &task_num, &task_arrival,
            &task_cpu);
        temp_task = new_task();
        temp_task->id = task_num;
        temp_task->arrival_time = task_arrival;
        temp_task->cpu_request = task_cpu;
        temp_task->cpu_used = 0.0;
        event_list = add_end(event_list, temp_task);
    }

#ifdef DEBUG
    int num_events;
    apply(event_list, increment_count, &num_events);
    printf("DEBUG: # of events read into list -- %d\n", num_events);
    printf("DEBUG: value of quantum length -- %d\n", quantum_length);
    printf("DEBUG: value of dispatch length -- %d\n", dispatch_length);
#endif

    run_simulation(quantum_length, dispatch_length);

    return (0);
}
