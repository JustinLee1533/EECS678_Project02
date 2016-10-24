/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

/**
  Stores information making up a job to be scheduled including any statistics.
  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t
{
  //TODO: Liia do this (and justin?) first
    //we for sure need these
    int pid; //pid passed in
    int arrivalTime; //arrival time passed in
    int priority; //priority passed in
    int runningTime; //running time passed in
    int timeRemaining; //runningTime - time it has been executed
    


} job_t;

//need variables to calculate avg waiting time
//avg turnaround time
//avg response time
float totalWaitingTime; //total waiting time
float totalResponseTime; //total response time
float totalTATime; //total turnaround time
int numOfJobs; //number of jobs for the scheduler

//we have to do compare functions
//prototype: int comparer(const void *elem1, const void *elem2);
//FCFS, SJF, PSJF, PRI, PPRI, RR
/**
 * FCFS 
 * First come first serve compare function
 * 
 * needed? 
 */
int FCFScomparer(const void *a, const void *b){
   job_t *jobA = (job_t *)a;
   job_t *jobB = (job_t *)b;
   //we want ascending order for arrival time, so put a before b
   return (jobA->arrivalTime - jobB->arrivalTime);
}
/**
 * SJF
 * Shortest job first compare function
 */
int SJFcomparer(const void *a, const void *b){
   job_t *jobA = (job_t *)a;
   job_t *jobB = (job_t *)b;
   //we want ascending order for running time, so put a before b
   return (jobA->runningTime - jobB->runningTime);
}
/**
 * PSJF 
 * Preemptive shortest job first compare function
 * Schedule based on its remaining time, not full running time
 */
int PSJFcomparer(const void *a, const void *b){
   job_t *jobA = (job_t *)a;
   job_t *jobB = (job_t *)b;
   //if running time of A is less than the remaining time of B, schedule it before
   return (jobA->runningTime - jobB->timeRemaining);
}
/**
 * PRI
 * Priority compare function
 */
int PRIcomparer(const void *a, const void *b){
   job_t *jobA = (job_t *)a;
   job_t *jobB = (job_t *)b;
   //if the priorities are not the same
   if(jobA->priority != jobB->priority){
       //we want descending priority
       return (jobB->priority - jobA->priority);
   }
   //if the priorities are the same, go off of arrival time
   return (jobA->arrivalTime - jobB->arrivalTime);
}
/**
 * PPRI
 * Preemptive priority compare function
 */
int PPRIcomparer(const void *a, const void *b){
   job_t *jobA = (job_t *)a;
   job_t *jobB = (job_t *)b;
   //if the priorities are not the same
   if(jobA->priority != jobB->priority){
       //we want descending priority
       return (jobB->priority - jobA->priority);
   }
   //if the priorities are the same, go off of job time 
   return (jobA->runningTime - jobB->timeRemaining);
}
/**
 * RR
 * Round Robin compare function
 * 
 * needed? it's just FCFS
 */
int RRcomparer(const void *a, const void *b){
   job_t *jobA = (job_t *)a;
   job_t *jobB = (job_t *)b;
   //we want ascending order for RR, so put a before b
   return (jobA->arrivalTime - jobB->arrivalTime);
}


/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t (from the header)
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  //TODO: justin do this

}


/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
 //TODO: determine if multicore, if multiple are idle, the lowest core_id
 // is it premptive? if so preempt; if not TODO: return the index of the core of the job to be scheuled on
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  //TODO: justin do this

	return -1;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  //TODO: justin do this
    //TODO: update total waiting time
    //TODO: update total turn around time
    //TODO: update total response time
 

	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
  //TODO: Liia do this
	return totalWaitingTime/numOfJobs;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
  //TODO: Liia do this
	return totalTATime/numOfJobs;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
  //TODO: Liia do this
    return totalResponseTime / numOfJobs;
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  //TODO: Liia do this
    

}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  //TODO: Liia do this

}
