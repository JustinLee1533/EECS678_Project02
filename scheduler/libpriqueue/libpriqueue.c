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
    //set size to zero
    q->msize = 0;
    //set compare function to parameter
    q->comparer = comparer;
    //front and back point to null
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
    //if queue is empty
    if(q->msize == 0)
    {
        //make a new node
        node_t *temp = malloc(sizeof(node_t));
        //set temp's member variables
        temp->mvalue = ptr;
        temp->mnext = NULL;
        //mfront is now temp
        q->mfront = temp;
        //increase size
        q->msize++;
        //return the zero-based index, so zero since it's the first one
        return 0;
    } 
    //if the queue contains at least one node we have to compare the ptr to
    else 
    {
        //create a new node
        node_t *temp = malloc(sizeof(node_t));
        //get a pointer to the front
        node_t *current = q->mfront;
        //keep track of preceding node
        node_t *prev = NULL;
        //set temp's member variables
        temp->mvalue = ptr;
        temp->mnext = NULL;
        //this is what we return
        int index = 0;
        
        //this is a temp value to get the node's value as we slide through the queue
        void *slidePtr; 
        //slide through the queue using current
        //in loop, set prev to current as you slide through so you know if you're at front of the list
        while(current != NULL){
            //if temp should come before the current node, we found where temp needs to go
            if(q->comparer((temp->mvalue), (current->mvalue)) < 0){
                //while the current pointer isn't null, we have to shift the remaining elements down
                while(current != NULL){
                    //copy value
                    slidePtr = current->mvalue;
                    //set the current value to new node's value
                    current->mvalue = temp->mvalue;
                    //set temp's value to current's old value
                    temp->mvalue = slidePtr;
                    //if we are at the last element in the queue, set the last element, then exit loop
                    if(current->mnext == NULL){
                        //the way we just copied pointer values, temp goes at the end
                        current->mnext = temp;
                        //break so we don't go off the queue
                        break; 
                    }
                    //move current to the next element
                    current = current->mnext;
                }
                //break out of the outter whileloop because we found where temp goes
                break;
            }
            //increase the index we are looking at
            index++;
            //if we reached the end of the queue and never found where temp should go, temp goes at the end
            if(current->mnext == NULL){
                //set temp as the last element now
                current->mnext = temp;
                //break out of whileloop so we don't go off the queue
                break;
            }
            //move on to next value
            current = current->mnext;
            
        }
        //increase size, return the index of where we inserted the new node
        q->msize++;
        return index;
    }
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
        //if the queue isn't empty, return the front (head)
        if(q->msize != 0)
        {
        
            return q->mfront;
        }
        //otherwise return null
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
    //if the queue has at least one element
    if(q->msize > 0){
        //set temp to the front
        node_t *temp = q->mfront;
        //if the queue is bigger than one
        if(q->mfront->mnext != NULL){
            //set the new front
            q->mfront = q->mfront->mnext;
        } else {
            //otherwise set to null
            q->mfront = NULL;
        }
        //get temp's value
        void *tempReturn = temp->mvalue;
        //delete
        free(temp);
        //decrease size
        q->msize--;
        //return the value
        return  tempReturn;
    }
    //otherwise return null
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
        //if the index is greater than zero, attempt to find the value at the given index
        if (index >= 0){
            //current index
            int i = 0;
            //set temp to the front
            node_t *temp = q->mfront;
            //while we haven't gone off the list
            while(temp != NULL){
                //if current index is the desired index
                if( i == index){
                    //return temp's value
                    return temp->mvalue;
                }
                //go to the next element at the next index
                temp = temp->mnext;
                i++;
            }
            
            
        }
        //if index was not in the range of the queue, return null
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
    //if queue is empty
    if(q->msize == 0){
	return 0;
    } else {
        //the count of entries removed after we're done
        int count = 0;
        //set our temporary variables
        node_t *current = q->mfront;
        node_t *prev = q->mfront;
        //if the node containing ptr is at the front and 0 or more matching follow, take care of all matching pointers at the front so front points to the correct element
        while(current != NULL && current->mvalue == ptr){
            //get next
            q->mfront = current->mnext;
            //delete
            free(current);
            //move current to the next element
            current = q->mfront;
            //decrease size and increase count of nodes removed
            q->msize--;
            count++;
        }
        //all others in the list
        while(current != NULL){
            //continue while the value is not equal to the pointer
            while(current!=NULL && current->mvalue != ptr){
                //update prev and current
                prev = current;
                current = current->mnext;
            }
            //if we've reached the end of the list, return with the count
            if(current == NULL){
            
                return count;
            }
            //found a matching element
            //connect the previous with current's next to bridge the gap
            prev->mnext = current->mnext;
            //delete current
            free(current);
            //set current to the next element
            current = prev->mnext;
            //increase count, decrease size
            count++;
            q->msize--;
        }
        //all done, return count
        return count;
    }
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
        //set up temporary nodes
        node_t *current;
        node_t *next;
        node_t *prev;
        //if queue isn't empty
        if(q->msize > 0){
            //check if the element at the index at the queue isn't null
            if(priqueue_at(q, index) != NULL){
                //if not, set temp equal to the value
                node_t *temp = priqueue_at(q, index);
                //set the value to return at temp
                void *tempReturn = temp->mvalue;
                //if there's one element in the queue
                if(temp == q->mfront && temp == q->mback){
                
                    q->mfront == NULL;
                    q->mback == NULL;
                    //if temp is the front
                } else if (temp == q->mfront){
                    //set front to the next element
                    q->mfront = temp->mnext;
                } else if (temp == q->mback){
                    //set new back
                    q->mback= priqueue_at(q, index - 1);
                    q->mback->mnext = NULL; //check if neccessary 
                } else { //if element is somewhere else in the list
                    //set previous to the element at the previous index
                    prev = priqueue_at(q, index - 1);
                    //set next to the element at the next index
                    next = priqueue_at(q, index + 1);
                    //connect the two
                    prev->mnext = next;
                }
                //delete temp, decrease size, return temp's value
                free(temp);
                q->msize --; 
                return tempReturn;
                
            }
        }
	return NULL;
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
    //if there are elements to destroy 
    if (q->msize > 0){
        //set temp to front, get a next value
        node_t *temp = q->mfront;
        node_t *next = temp;
        //while there are still elements
        while(temp != NULL){
            //set next to temp's next
            next = temp->mnext;
            //delete temp
            free(temp);
            //set temp to the next element
            temp = next;
            //decrease size
            q->msize--;
        }
    }
}
