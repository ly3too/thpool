/*
* @author: ly3too
* @Description: example program of using thread pool
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "thpool.h"

void task(void *arg) {
    unsigned int t;
    printf("thread %I64u is doing task %s \n", pthread_self(), (char *)arg);
    t = (rand() * 1000) % 2000000; // sleep within 2s
    usleep(t);
}

int main(int argc, char **argv) {
    char *arg;

    setbuf(stdout, NULL);
    DEBUG_FUNC(printf, ("enter main\n"));

    srand(time(NULL));
    if(thpool_init(8, 100) != 0) {
        perror("thread pool init failure.");
        exit(EXIT_FAILURE);
    }

    //sleep(5);

    for (int i=0; i<200; ++i) {
        arg = (char *) malloc(20 * sizeof(char));
        if (! arg) {
            perror("malloc error.");
            exit(EXIT_FAILURE);
        }
        sprintf(arg, "%d", i);

        thpool_add_job(task, arg);
    }

    thpool_destroy();
}
