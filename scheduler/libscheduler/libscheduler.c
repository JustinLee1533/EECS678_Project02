/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

priqueue_t q;

//function pointers
int (* Comparer_ptr) (const void *, const void *) = NULL;

/**
  Stores information making up a job to be scheduled including any statistics.
  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t
{
    int pid; //pid passed in
    int arrivalTime; //arrival time passed in
    int priority; //priority passed in
    int runningTime; //running time passed in
    int timeRemaining; //runningTime - time it has been executed
    int core; // zero indexed core on which the job is running, -1 if idle
    int lastScheduled; //when the job was last scheduled to run
    int waitTime;
    int responseTime;
    int turnAroundTime;

} job_t;

/*
  array for cores, stores bools of whether a job is running on the core of that
  index or not
*/
job_t* *coreArr;

//number of cores we're using
int numCores = 0;

scheme_t schedScheme;

float totalWaitingTime; //total waiting time
float totalResponseTime; //total response time
float totalTATime; //total turnaround time
int numOfJobs; //number of jobs for the scheduler

/**
 * FCFS
 * First come first serve compare function
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
   return (jobA->timeRemaining - jobB->timeRemaining);
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
       return (jobA->priority - jobB->priority);
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
       return (jobA->priority - jobB->priority);
   }
   //if the priorities are the same, go off of job time
   return (jobA->arrivalTime - jobB->arrivalTime);
}
/**
 * RR
 * Round Robin compare function
 *
 * needed? it's just FCFS
 */
int RRcomparer(const void *a, const void *b){
   //We want the job to go to the back of the queue, so don't judge off of any criteria
   return (1);
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
    totalWaitingTime = 0.0; //total waiting time
    totalResponseTime = 0.0; //total response time
    totalTATime = 0.0; //total turnaround time
    numOfJobs = 0;
    //set the Comparer_ptr to proper function
    switch(scheme)
    {
      case FCFS :
        Comparer_ptr = FCFScomparer;
        break;

      case SJF :
        Comparer_ptr = SJFcomparer;
        break;

      case PSJF :
        Comparer_ptr = PSJFcomparer;
        break;

      case PRI :
        Comparer_ptr = PRIcomparer;
        break;

      case PPRI :
        Comparer_ptr = PPRIcomparer;
        break;

      case RR :
        Comparer_ptr = RRcomparer;
        break;
    }

    //initialze priorityqueue with
    priqueue_init(&q, Comparer_ptr);

    /*
      setup and initialize cores to false
    */
    coreArr = malloc(cores*sizeof(job_t));

    //set the global variable for cleanup later
    numCores = cores;

    schedScheme = scheme;
    //initialize the coreArr
    for(int i = 0; i<cores; i++)
      coreArr[i] = NULL;

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

 // is it premptive? if so preempt;
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  //TODO: justin do this
    job_t *temp = malloc(sizeof(job_t));
    temp->pid = job_number;
    temp->arrivalTime = time;
    temp->runningTime = running_time;
    temp->timeRemaining = running_time;
    temp->priority = priority;

    temp->responseTime = -1;

    //single core
    if(numCores == 1)
    {
      switch(schedScheme)
      {
        //non-preemptive
        case FCFS :
        case SJF :
        case PRI :
            if(coreArr[0] == NULL) {
             //if not make it run on the core
             coreArr[0] = temp;
             coreArr[0]->responseTime = time - coreArr[0]->arrivalTime;
             coreArr[0]->lastScheduled = time;
             return(0);
            } else {
                priqueue_offer(&q, temp);
                return(-1);
            }
        break;

        //Preemptive
        //check premption condition
        case PPRI :
            if(coreArr[0] == NULL) {
             //if not make it run on the core
             coreArr[0] = temp;
             coreArr[0]->responseTime = time - coreArr[0]->arrivalTime;
             return(0);
            } else {
                //if new job is of higher priority than job currently running on core
                if( priority < coreArr[0]->priority || (priority == coreArr[0]->priority && coreArr[0]->arrivalTime > time)){
                    //stop current job on core, put on queue
                    if(coreArr[0]->lastScheduled == time){

                        coreArr[0]->responseTime = -1;
                    }
                    priqueue_offer(&q, coreArr[0]);
                    coreArr[0] = temp;
                    coreArr[0]->responseTime = time - coreArr[0]->arrivalTime;
                    return(0);

                } else {
                    priqueue_offer(&q, temp);
                    return(-1);
                }
            }
            break;
        case PSJF :
            if(coreArr[0] == NULL) {
             //if not make it run on the core
             coreArr[0] = temp;
             coreArr[0]->responseTime = time - coreArr[0]->arrivalTime;
             coreArr[0]->lastScheduled = time;
             return(0);
            } else {
              int timeDiff = time - coreArr[0]->lastScheduled;
              coreArr[0]->timeRemaining -= timeDiff;

              /*
                if the time difference is greater than the runtime of the new
                job, then schedule the new job
              */

              if(coreArr[0]->timeRemaining > running_time)
              {
                  if(coreArr[0]->responseTime == time - coreArr[0]->arrivalTime){

                        coreArr[0]->responseTime = -1;
                  }
                //remove job from core
                //update its timeRemaining,
                //add old job back to the queue
                  priqueue_offer(&q, coreArr[0]);

                //assign new job to the core
                coreArr[0] = temp;
                coreArr[0]->lastScheduled = time;
                coreArr[0]->responseTime = time - coreArr[0]->arrivalTime;
                return(0);
              }else
              {
                //add new job to the priority queue
                priqueue_offer(&q, temp);
                return(-1);
              }
            }
            break;
        case RR :
            if(coreArr[0] == NULL) {
             //if not make it run on the core
             coreArr[0] = temp;
             coreArr[0]->responseTime = time - coreArr[0]->arrivalTime;
             coreArr[0]->lastScheduled = time;
             return(0);
            } else {
              priqueue_offer(&q, temp);
            }

            break;
      }
    } else {
        //Multicore 
        int coreIndex = -1;
        //look for an open core
        for(int i = 0; i < numCores; i++){
            if(coreArr[i] == NULL){
                //found one, break
                coreIndex = i;
                break;
            }
        }
        //found a core to run on
        if(coreIndex != -1){
            coreArr[coreIndex] = temp;
            coreArr[coreIndex]->responseTime = time - coreArr[coreIndex]->arrivalTime;
            if(schedScheme == PSJF){
              coreArr[coreIndex]->lastScheduled = time;
            }
            return(coreIndex);
        } else { //otherwise we have to schedule

            //only used in PSJF;
            int lowestPriority;
            int lowestIndex;
            int tie;
            switch(schedScheme)
            {
                //non-preemptive
                case FCFS :
                case SJF :
                case PRI :
                    priqueue_offer(&q, temp);
                    return (-1);
                break;

                //Preemptive
                //check premption contidtion
                case PSJF :
                //update time difference

                //first time update
                coreArr[0]->timeRemaining -=time - coreArr[0]->lastScheduled;
                coreArr[0]->lastScheduled = time;


                int highestRemTime = coreArr[0]->timeRemaining;
                int highestIndex = 0;
                // int lowestArrivalTime = coreArr[0]->arrivalTime;
                // int lowestArrivalTimeIndex = 0;

                //update remaining times and find lowest remaining time
                for(int i = 1; i < numCores; i++)
                {
                    //calculate the new remaining time
                    //int timeDiff = time - coreArr[i]->lastScheduled;
                    //int timeDiff = time - prevTime;
                    coreArr[i]->timeRemaining -= time - coreArr[i]->lastScheduled;
                    coreArr[i]->lastScheduled = time;

                    //see if the coreArr[i] remaining time is < than highestRemTime
                    if(coreArr[i]->timeRemaining > highestRemTime)
                    {
                      highestIndex = i;
                      highestRemTime = coreArr[i]->timeRemaining;
                    }
                }
              //  prevTime = time;

                //check if the lowest remaining time in the coreArr is greater
                //than  the new job, if so, assign it to that core
                if(highestRemTime > running_time)
                {

                  if(coreArr[highestIndex]->responseTime == (time - coreArr[highestIndex]->arrivalTime))
                  {
                    coreArr[highestIndex]->responseTime = -1;
                  }
                  priqueue_offer(&q, coreArr[highestIndex]);
                  coreArr[highestIndex] = temp;
                  //coreArr[highestIndex]->lastScheduled = time;

                  if(coreArr[highestIndex]->responseTime == -1)
                    coreArr[highestIndex]->responseTime = (time - coreArr[highestIndex]->arrivalTime);
                  return(highestIndex);
                } else {
                  priqueue_offer(&q, temp);
                  return -1;
                }
                break;
                case PPRI :
                    lowestPriority = coreArr[0]->priority;
                    lowestIndex = 0;
                    tie = 0;
                    for(int i = 1; i < numCores; i++){

                        //if lower priority then update lowest priority and lowest index
                        if(coreArr[i]->priority > lowestPriority){

                            lowestPriority = coreArr[i]->priority;
                            lowestIndex = i;
                            tie = 0;
                        } else if (coreArr[i]->priority == lowestPriority){

                            if(coreArr[i]->arrivalTime > coreArr[lowestIndex]->arrivalTime){

                                    lowestIndex = i;
                            }
                        }
                    }
                    if(lowestPriority > priority){
                        if(coreArr[lowestIndex]->lastScheduled == time){

                            coreArr[lowestIndex]->responseTime = -1;
                        }
                        priqueue_offer(&q, coreArr[lowestIndex]);
                        coreArr[lowestIndex] = temp;
                        coreArr[lowestIndex]->responseTime = time - coreArr[lowestIndex]->arrivalTime;
                        return lowestIndex;

                    } else if (lowestPriority == priority){

                        if(tie != 0){
                            //start at lowest index and see which has the better run time
                            for(int i=lowestIndex; i < numCores; i++){

                                if(coreArr[i]->arrivalTime > coreArr[lowestIndex]->arrivalTime){
                                    lowestIndex = i;
                                }
                            }
                            if(coreArr[lowestIndex]->arrivalTime > time){
                                if(coreArr[lowestIndex]->lastScheduled == time){

                                    coreArr[lowestIndex]->responseTime = -1;
                                }
                                priqueue_offer(&q, coreArr[lowestIndex]);
                                coreArr[lowestIndex] = temp;
                                coreArr[lowestIndex]->responseTime = time - coreArr[lowestIndex]->arrivalTime;
                                return lowestIndex;
                            } else {
                                priqueue_offer(&q, temp);
                                return -1;
                            }
                        } else {
                            if(coreArr[lowestIndex]->arrivalTime > time){
                                if(coreArr[lowestIndex]->lastScheduled == time){

                                    coreArr[lowestIndex]->responseTime = -1;
                                }
                                priqueue_offer(&q, coreArr[lowestIndex]);
                                coreArr[lowestIndex] = temp;
                                coreArr[lowestIndex]->responseTime = time - coreArr[lowestIndex]->arrivalTime;
                                return lowestIndex;
                            } else {
                                priqueue_offer(&q, temp);
                                return -1;
                            }
                        }
                    } else {
                        priqueue_offer(&q, temp);
                        return -1;
                    }

                    break;
                case RR :
                    priqueue_offer(&q, temp);
                    return -1;
                    break;
            }
        }
    }
	return -1;
}


/**
  Called when a job has completed execution.
-
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
-
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time) {
    //printf("\n\n\nRESPONSE TIME JOB %d is %d\n\n\n", coreArr[core_id]->pid, coreArr[core_id]->responseTime);
    totalResponseTime += coreArr[core_id]->responseTime;
    totalWaitingTime += time - coreArr[core_id]->arrivalTime - coreArr[core_id]->runningTime;
    totalTATime +=time - coreArr[core_id]->arrivalTime;
    numOfJobs++;
    free(coreArr[core_id]);
    coreArr[core_id] = NULL;
    //if there's still a job to be done
    if(priqueue_size(&q) > 0){
        //get the next job
        job_t* temp = (job_t*)priqueue_poll(&q);
        //will have to do something for psjf
        coreArr[core_id] = temp;
        //set the response time that it's now been scheduled
        if(coreArr[core_id]->responseTime == -1) {
            coreArr[core_id]->lastScheduled = time;
            coreArr[core_id]->responseTime = time - coreArr[core_id]->arrivalTime;
        }
        if(schedScheme == PSJF){
          coreArr[core_id]->lastScheduled = time;
            //printf("\n\n\nSCHEDULED JOB %d is %d\n\n\n", coreArr[core_id]->pid, coreArr[core_id]->responseTime);
            if(coreArr[core_id]->responseTime == -1){
                    coreArr[core_id]->responseTime = time - coreArr[core_id]->arrivalTime;
                  //  coreArr[core_id]->lastScheduled = time;
                   // printf("\n\n\nSCHEDULED JOB %d is %d\n\n\n", coreArr[core_id]->pid, coreArr[core_id]->responseTime);
            }
        }
      return coreArr[core_id]->pid;
    }

    return -1;
}

/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
-
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.
-
  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
    //only on the one core
    //job on the core
    job_t *temp = coreArr[core_id];
    //if there's no job currently running on the core
    if(temp == NULL) {
        //if there's no job waiting in the queue
        if(priqueue_size(&q) == 0){
            return -1;
        }
    } else {
        //otherwise put temp in the back of the queue
        priqueue_offer(&q, temp);
    }
    //get the next job on the queue to begin running on the core
    coreArr[core_id] = priqueue_poll(&q);
    //if job hasn't yet been run
    if(coreArr[core_id]->responseTime == -1){
        //response = current time - arrival time
        coreArr[core_id]->responseTime = time - coreArr[core_id]->arrivalTime;
    }
    return coreArr[core_id]->pid;
}

/**
  Returns the average waiting time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return totalWaitingTime / numOfJobs;
}

/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
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
    return (totalResponseTime / numOfJobs);
}

/**
  Free any memory associated with your scheduler.
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  //TODO: Liia do this
  //Free the core array
  free(coreArr);
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
  node_t* temp = q.mfront;
  for(int i = 0; i< priqueue_size(&q); i++)
  {
    //print job and the core that its running on
    job_t* valptr = (job_t*)(temp->mvalue);
    //*job_t coreptr = (*job_t)(temp->mvalue);

    printf("   %d (%d) ", valptr->pid, valptr->core);
    temp = temp->mnext;
  }
}
