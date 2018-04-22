/*
* @author: ly3too
* @Description: example program of using thread pool
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "thpool.h"

void task(void *arg) {
    printf("thread %I64u is doing task %s", pthread_self(), (char *)arg);
    sleep(1);
}

int main(int argc, char **argv) {
    char *arg;
    if(thpool_init(5, 1024) != 0) {
        perror("thread pool init failure.");
        exit(EXIT_FAILURE);
    }

    printf("init ok\n");

    for (int i=0; i<1024; ++i) {
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
