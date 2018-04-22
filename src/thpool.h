/**********************************
 * @author      ly3too
 *
 **********************************/

#ifndef _THPOOL_
#define _THPOOL_

#ifdef __cplusplus
extern "C" {
#endif

/**/
int thpool_init(int num_th, int que_size);

/* add job to thread pool */
int thpool_add_job(void (func*) (void *arg), void *arg);

/* wait until jobs in que to finish and free thread pool */
void thpool_destroy();

#ifdef __cplusplus
}
#endif
#endif