/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_


/**
 *  Node Structure
 */
typedef struct node_t node_t;

struct node_t
{
    void *mvalue;
    node_t *mnext;
    
};

/**
  Priqueue Data Structure
*/
typedef struct _priqueue_t
{
    int msize; //make sure this doesn't need to be size_t
    int(*comparer)(const void *, const void *);
    node_t *mfront;
    node_t *mback; //make sure this is neccessary
    
} priqueue_t;


void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
