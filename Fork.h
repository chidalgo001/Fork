// Fork.h

/***************************************************/
/* Author: Christian Hidalgo. PID: 2103434         */

/* Description: User defined header file that contains the appropiate constants
   and function prototypes */

/* Input: N.A */
/* Output: N.A. */

/* I certify that this work is my own and none of it is work of any other person or entity 	*/



/**********************************************************************************	*/
/* Program Defines */

# include <unistd.h>            //For the use of fork()
# include <stdio.h>             //Standard I/O
# include <sys/wait.h>          //For the use of wait()
# include <stdlib.h>            //For the use of atoi() and exit()
# include <sys/shm.h>           //For the use of shared memory functions
# include <sys/ipc.h>           //For the use of IPC
# include <string.h>            //For the use of strlen

# define MAXLENGTH 1            //Allowed inputs of length 1 ( since valid input is 1 digit)
# define EXPECTED_ARGS_MIN 2    //MIN Number of expected arguments from command-line
# define EXPECTED_ARGS_MAX 8    //MAX Number of expected arguments from command-line
# define PARENT 0               //Parent identifier
# define MIN_NUMBER 48          //Lower range of numbers accepted (ASCII 0)
# define MAX_NUMBER 57          //Higest range of numbers accepted (ASCII 9)
# define PASS 0                 //Return value if any validation passes
# define FAILED -1              //Return value if any validation fails and termination
# define FLAG 0666              //Code for the function shmget in order to get shared memory.

/**********************************************************************************	*/
/* Function prototypes	*/


int validateNumber( char );
int validateArguments( int );
int validateInput( char *argv[] , int );
int getUniqueID(int , int , int[]);
void displaySharedMemory( int * , int , int );
void displayArgError(void);
void fillMemory( int * , char *[] , int );
void childProcess( int , int , int * , pid_t );

