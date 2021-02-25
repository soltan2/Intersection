/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
 */

/*
 * Includes
 *
 */
#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>
#include <machine/spl.h>

/*
 *
 * Constants
 *
 */

/*
 * Number of vehicles created.
 */
#define NVEHICLES 20

struct lock *AB;
struct lock *BC;
struct lock *CA;
struct lock *temp;
struct lock *Atruck,*Btruck,*Ctruck;/*trucks on A,B,and C routes.*/
static const char *positions[] = { "Route A", "Route B", "Route C","AB","BC","CA"};
static const char *vtype[]={"Car","Truck"};
static const char *status[] = { "approaches from", "enters the portion", "leaves through"};
static const char *turns[] = {"left","right"};
enum{LEFT,RIGHT};/*LEFT==0 and RIGHT==1 can be used instead of nums*/
enum{CAR,TRUCK}; 
enum{A,B,C,ab,bc,ca};
enum{approach,intersect,leave};
volatile int count=0; /* count the total number of vehicles (cars and trucks that have passed*/
void wait(unsigned long vehicletype); /* a function to make trucks wait by putting them to sleep*/
void yield(unsigned long vehicledirection); /*waking up the trucks*/
void destroyLocks();
int *numcars;/*int array storing number of cars on a given route*/


/*
 *
 * Function Definitions
 *
 */

/*printing function receives vehicletype, vehiclenumber, status, position, the turn and destination*/
static void trackmsg(unsigned long type,unsigned long vehiclenumber, int stage, int position, int turn, int dest)
{
  kprintf("%-5s Number: %-2lu %s %-3s [turning %s, destination: %s]\n", vtype[type], vehiclenumber,status[stage], positions[position],turns[turn], positions[dest]);
}

/*
 * turnleft()q
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static void turnleft(unsigned long vehicledirection,unsigned long vehiclenumber,unsigned long vehicletype)
{
	/*
	 * Avoid unused variable warnings.
	 */

	(void) vehicledirection;
	(void) vehiclenumber;
	(void) vehicletype;
   

  if(vehicledirection==A) {
   
	  lock_acquire(AB);
    trackmsg(vehicletype,vehiclenumber,intersect,ab,LEFT,C);
    lock_acquire(BC);
	  trackmsg(vehicletype,vehiclenumber,intersect,bc,LEFT,C);
    lock_release(AB);
    trackmsg(vehicletype,vehiclenumber,leave,C,LEFT,C);
    lock_release(BC);
    
    
  }else if(vehicledirection==B){
  
    lock_acquire(CA);
    lock_acquire(BC); 
	  trackmsg(vehicletype,vehiclenumber,intersect,bc,LEFT,A);
    trackmsg(vehicletype,vehiclenumber,intersect,ca,LEFT,A);
    lock_release(BC);
    trackmsg(vehicletype,vehiclenumber,leave,A,LEFT,A);
    lock_release(CA);
    
  }else{

    lock_acquire(CA); 
    trackmsg(vehicletype,vehiclenumber,intersect,ca,LEFT,B);
    lock_acquire(AB);
    trackmsg(vehicletype,vehiclenumber,intersect,ab,LEFT,B);
    lock_release(CA);
    trackmsg(vehicletype,vehiclenumber,leave,B,LEFT,B);
    lock_release(AB);
    
  }
   
}

/*
 * turnright()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static void turnright(unsigned long vehicledirection,unsigned long vehiclenumber,unsigned long vehicletype)
{
	/*
	 * Avoid unused variable warnings.
	 */
     	(void) vehicledirection;
      (void) vehiclenumber;
    	(void) vehicletype;
 
    if(vehicledirection==A) { 
    
  	  lock_acquire(AB); 
      trackmsg(vehicletype,vehiclenumber,intersect,ab,RIGHT,B);
      trackmsg(vehicletype,vehiclenumber,leave,B,RIGHT,B);
      lock_release(AB);
  
    }else if(vehicledirection==B) { 
   
  	  lock_acquire(BC); 
      trackmsg(vehicletype,vehiclenumber,intersect,bc,RIGHT,C);
      trackmsg(vehicletype,vehiclenumber,leave,C,RIGHT,C);
      lock_release(BC);
     
    }
    else{
    
  	  lock_acquire(CA); 
      trackmsg(vehicletype,vehiclenumber,intersect,ca,RIGHT,A);
      trackmsg(vehicletype,vehiclenumber,leave,A,RIGHT,A);
      lock_release(CA);
    }
 
 
}


/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long vehiclenumber: holds vehicle id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createvehicles().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */

static void approachintersection(void * unusedpointer, unsigned long vehiclenumber)
{
	int vehicledirection, turndirection, vehicletype;
	/*
	 * Avoid unused variable and function warnings.
	 */
	(void) unusedpointer;
	(void) vehiclenumber;
	(void) turnleft;
	(void) turnright;
	/*
	 * vehicledirection is set randomly.
	 */
	vehicledirection = random() % 3;
	turndirection = random() % 2;
	vehicletype = random() % 2;
 
 /*
  * Determining the destination based on the turn. 
  */
  int dest=0;
  if(turndirection==LEFT){
    dest=(vehicledirection+5)%3;
  }else{
    dest=(vehicledirection+1)%3;
  }
    
   if(vehicletype==CAR){
   
       numcars[vehicledirection]+=1; /* increment the number of cars on the route used.*/
    	if(turndirection==LEFT){
       trackmsg(vehicletype,vehiclenumber,approach,vehicledirection,turndirection,dest);/*print the approach meassage*/
       turnleft(vehicledirection, vehiclenumber, vehicletype);
       }
       else{
       trackmsg(vehicletype,vehiclenumber,approach,vehicledirection,turndirection,dest);
       turnright(vehicledirection, vehiclenumber, vehicletype);
       }
       numcars[vehicledirection]-=1; /*decrement the num of cars on the route. */
       if(numcars[vehicledirection]==0){ /*if there are no cars,trucks can proceed. */
        yield(vehicledirection);
       }  
   }else{    
       if(turndirection==LEFT){  
            trackmsg(vehicletype,vehiclenumber,approach,vehicledirection,turndirection,dest);/*print approach message */
            /*check for cars before proceeding*/
            if(numcars[vehicledirection] > 0) {  
              wait(vehicledirection); /*truck threads are put to sleep if there are cars on the same root. */
            }
             turnleft(vehicledirection, vehiclenumber, vehicletype);
       }else{
            trackmsg(vehicletype,vehiclenumber,approach,vehicledirection,turndirection,dest);
            if(numcars[vehicledirection] > 0) {
              wait(turndirection);
            }
            turnright(vehicledirection, vehiclenumber, vehicletype);
       }   
   }
   /*vehicles passing counter. Once done, we wake up the menu thread.*/
    count++;
  if(count == NVEHICLES) {
      count=0;
			int spl = splhigh();
			thread_wakeup(temp);
			splx(spl);
  }
}
/*wake up a truck based on its direction*/
void yield(unsigned long lane)
{
  int spl=splhigh();
   switch(lane){
     case 0:
      thread_wakeup(Atruck);
      break;
    case 1:  
      thread_wakeup(Btruck);
      break;
    case 2:
      thread_wakeup(Ctruck);
      break;     
   }
   splx(spl);
}
/*Put a truck to sleep based on its direction*/
void wait(unsigned long lane)
{
  int spl=splhigh();
   switch(lane){
    case 0:
     thread_sleep(Atruck);
     break;
    case 1: 
      thread_sleep(Btruck);
      break;
    case 2:
      thread_sleep(Ctruck);
      break;
   }
   splx(spl); 
}

/*destroying all locks used*/
void destroyLocks(){

	 lock_destroy(AB);
   lock_destroy(BC);
   lock_destroy(CA);
	 lock_destroy(temp);
   lock_destroy(Atruck);
   lock_destroy(Btruck);
   lock_destroy(Ctruck);
  
}



/*
 * createvehicles()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int createvehicles(int nargs,char ** args)
{
	int index, error;

	/*
	 * Avoid unused variable warnings.
	 */
	(void) nargs;
	(void) args;
  /* creating all locks needed*/
  AB = lock_create("AB");
  BC = lock_create("BC");
  CA = lock_create("CA");
  temp=lock_create("temp");
  Atruck = lock_create("Atruck");
  Btruck = lock_create("Btruck");
  Ctruck = lock_create("Ctruck");
  /*an int array to keep track of numcars on each of the three routes*/
  numcars = (int *)kmalloc(3 * sizeof(int));
	*(numcars) = 0;
	*(numcars + 1) = 0;
	*(numcars + 2) = 0;

	/*
	 * Start NVEHICLES approachintersection() threads.
	 */
	for (index = 0; index < NVEHICLES; index++) {

		error = thread_fork("approachintersection thread",
				NULL,
				index,
				approachintersection,
				NULL
				);

		/*
		 * panic() on error.
		 */

		if (error) {

			panic("approachintersection: thread_fork failed: %s\n",
					strerror(error)
				 );
		}
	}
   /*
   *waiting for all the cars to pass. 
   */
   int spl = splhigh();
   if(count < NVEHICLES) {
  		thread_sleep(temp);
  	}
  	splx(spl);
 
   destroyLocks();
   kfree(numcars);
 
  
	return 0;
}
