/* ********************************
 * Author:      ly3too

 * Description:
 ********************************/

#include "thpool.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static volatile int keepalive; // enable main loop
static volatile int num_threads; // number of worker threads
pthread_t *p_threads;  // record threads' ID in pool
sem_t has_jobs, job_slots; // producer consumer model
pthread_cond_t finished_jobque; // free thpool wati condition

typedef struct job{
    struct job *next;
    void (*func) (void *arg);
    void * arg;
} job_t;

typedef struct {
    pthread_mutex_t lock;
    job_t * head;
    job_t * tail;
    int len;
} jobque_t;

jobque_t jobque;

void free_job(job_t * job) {
    free(job -> arg);
    free(job);
}

int jobque_init(jobque_t* p_que) {
    pthread_mutex_lock(&(p_que -> lock));
    p_que -> head = NULL;
    p_que -> tail = NULL;
    p_que -> len = 0;
    pthread_mutex_unlock(&(p_que -> lock));
    return 0;
}

int jobque_push(jobque_t* p_que, job_t *p_job) {
    pthread_mutex_lock(&(p_que -> lock));
        if (p_que -> len == 0) {
            p_que -> head = p_que -> tail = p_job;
        } else {
            (p_que -> tail) -> next = p_job;
            p_job -> next = NULL;
            p_que -> tail = p_job;
        }
        ++ (p_que -> len);
    pthread_mutex_unlock(&(p_que -> lock));
    return 0;
}

job_t *jobque_pop(jobque_t* p_que) {
    job_t * new_job = NULL;
    pthread_mutex_lock(&(p_que -> lock));

    if (p_que -> len > 0) {
        new_job = p_que -> head;
        p_que -> head = new_job -> next;
        new_job -> next = NULL;
        -- p_que -> len;
    }

    pthread_mutex_unlock(&(p_que -> lock));
    return new_job;
}

int jobque_size(jobque_t* p_que) {
    int size = 0;
    pthread_mutex_lock(&(p_que -> lock));
    size = p_que -> len;
    pthread_mutex_unlock(&(p_que -> lock));
    return size;
}

void jobque_destroy(jobque_t* p_que) {
    pthread_mutex_lock(&(p_que -> lock));

    while (p_que) {
        job_t *job = p_que -> head;
        p_que -> head = p_que -> head -> next;
        free_job(job);
    }
    p_que -> len = 0;

    pthread_mutex_unlock(&(p_que -> lock));
    pthread_mutex_destroy(p_que -> lock);
}

/* main worker threads */
void* worker (void *arg) {
    while (keepalive) {
        sem_wait(has_jobs);
        job_t *new_job = jobque_pop(&jobque);
        if (new_job != NULL) {
            sem_post(job_slots);
            new_job->func(new_job->arg);
            free_job(new_job);
        } else {
            pthread_cond_signal(&finished_jobque);
        }
    }

    return NULL;
}

/* add job to jobque, arg will be freed after task finshed */
int thpool_add_job(void (*func) (void *arg), void *arg) {
    sem_wait(job_slots);
    job_t *new_job = (job_t *)malloc(sizeof(job_t));
    if (new_job == NULL) {
        perror("cannot creat job");
        return -1;
    }

    new_job -> func = func;
    new_job -> arg = arg;
    new_job -> next = NULL;
    if (jobque_push(&jobque, new_job)){
        perror("cannot add new job");
        free_job(new_job);
        return -1;
    }
    sem_post(has_jobs);

    return 0;
}


/* return -1 if anny error */
int thpool_init(int num_th, int que_size) {
    if (que_size <= 0 || num_th <= 0) {
        perror("que size and number of threads should be positive value");
        return -1;
    }

    keepalive = 1;
    num_threads = num_th;
    sem_init(&has_jobs, 0, 0);
    sem_init(&job_slots, 0, que_size);
    p_threads = (pthread_t *)malloc(num_th * sizeof(pthread_t));
    if (p_threads == NULL) {
        perror("cannot creat threads id array");
        return -1;
    }

    if (jobque_init(&jobque) < 0) {
        perror("jobque init failed");
        jobque_destroy(&jobque);
        return -1;
    }

    pthread_cond_init(&finished_jobque, NULL);
    /*creating process*/
    for (int i=0; i<num_th; ++i) {
        if (pthread_create(&(p_threads[i]), NULL, &worker, NULL) != 0) {
            perror("creating pthread failed");
            return -1;
        }
    }

    return 0;
}

/* wait jobs to be finished in jobque and free thread pool */
void thpool_destroy() {
    void* retval;
    sem_post(job_slots);
    pthread_cond_wait(&finished_jobque, NULL);
    keepalive = 0;
    for (int i=0; i<num_threads; ++i) {
        pthread_join(p_threads[i], &retval);
    }

    jobque_destroy(&jobque);
    free(p_threads);

    sem_destroy(has_jobs);
    sem_destroy(job_slots);
    pthread_cond_destroy(&finished_jobque);
}
