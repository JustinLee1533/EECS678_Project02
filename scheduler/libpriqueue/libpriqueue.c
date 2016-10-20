/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
    q->msize = 0;
    q->comparer = comparer;
    q->mfront = NULL;
    q->mback = NULL;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
    //returning the index
    //if queue is empty
    if(q->size == 0)
    {
        node_t *temp = malloc(sizeof(node_t));
        temp->mvalue = ptr;
        temp->mnext = NULL;
        q->mfront = temp;
        //see if mback is neccessary...
        q->mback = temp;
        q->msize++;
    
        return 0;
    } 
    //if the queue contains at least one node we have to compare the ptr to
    else 
    {
        node_t *temp = malloc(sizeof(node_t));
        node_t *current = q->mfront;
        //keep track of preceding
        node_t *prev = NULL;
        temp->mvalue = ptr;
        temp->mnext = NULL;
        int index = 0;
        //slide through the queue using current
        //in loop, set prev to current as you slide through so you know if you're at front of the list
        while(current != NULL){
            //this could be backwards... greater priority? less than?
            //if the ptr value is at a place we should insert, shift everything down
            //queue test compare1 , if a is larger, return positive value
            if(q->comparer((temp->mvalue), (current->mvalue)) < 0){
            
                //FINISH HERE!
                //put cases in here, if current is mfront, mback, or in middle
            }
        }
        
        q->msize++;
        return index;
    }
        //good check
	return -1;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
        if(q->size != 0)
        {
        
            return q->mfront;
        }
	return NULL;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
        //if q->msize > 0
        //remove
        //temp = mfront
        // tempval = temp->mvalue
        // mfront = mfront->mnext
        // q->msize--
        //free(temp) 
        //return tempval
        //don't forget to free tempval when it's a job in scheduler
	return NULL;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
        // I see no harm in calling priqueue_at on the head, but make sure it's okay
        if(index >= 0 && index < (q->size - 1)){
            int i = 0;
            node_t *temp = q->mfront;
            while(temp != NULL){
            
                if( i == index){
                
                    return temp->mvalue;
                }
                temp = temp->mnext;
                i++;
            }
            
            
        }
	return NULL;
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	return 0;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	return 0;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->msize;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
    if (q->msize > 0){
    
        node_t *temp = q->mfront;
        node_t *next = temp;
        while(temp != NULL){
            next = temp->mnext;
            free(temp);
            temp = next;
            q->msize--;
        }
    }
}
