# thpool - a simple thread pool based on producer consumer model

## APIs:
| API | Description |
| -------------------- | ------------------------- |
| ***int thpool_init(int num_th, int que_size);*** | initialize thread pool including job queue and variables used. num_th: number of threas. <br> que_size: maxim jobque size |
| ***int thpool_add_job(void (*func) (void *), void *arg);*** | add job to jobque, arg will be freed after task finshed, this will block the calling thread if jobque reaches que_size; <br>func: task to be done; <br> arg: argument passed to the task, it will be automaticly freed if the task finishes |
| ***void thpool_destroy();*** | wait until jobs in que to finish and free thread pool, this will block the calling thread if jobque is not empty|

## usage:
``` shell
cd src
make # to compile example program
make debug # to compile example in debug mode
```
