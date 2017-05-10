
// Fork.c

/***************************************************/
/* Author: Christian Hidalgo.       */

/* Description: Program, when executed is to accept 1 and up to 7 distinct inputs
   from the command line. once the program validates the inputs ( diferent digits
   between 0 and 9)it will request shared memory big enough to fit the input and will 
   fill it up with it. Once it has been done succesfully it will then fork that many children. 
   Each children is to display the state of the memory, then access its corresponding 
   shared memory space and will perform a math process with it and finally display the state of the 
   shared memory once more. at the end of this, the parent will be waiting until all children
   are done to finally terminate the program.*/

/* Input: digits between 0 and 9 in the commmand line ( possible number of inputs are 1 to 7) */

/* Output: Will be trace statements from both the parent and the children after each statement executed
   Parent statements will be tagged with "Parent: " and child statements will be tagged with "Child #: 
   The shared memory state will also be displayed various times " */

/* compilation: make or gcc Fork.c -o Fork.out */
/* execute with ./Fork.out */

/* I certify that this work is my own and none of it is work of any other person	*/
/* or entity (with exception of the temperature conversion formulas               	*/



/**********************************************************************************	*/
/* Program Defines */

# include "Fork.h" //For the use of constants and function prototypes

/**********************************************************************************	*/


int main( int argc , char *argv[] ){
    
    printf("\nParent: Process Begins...\n\n");
    
    /* int total is the number of inputs which is the argument count - 1 (To eliminate Magic Number) */
    int total = argc - 1;
    int *shmPointer;
    int result , count , shmID , holder, status;
    int IDlist[ argc ];// will hold in order the every child process ID
    
    pid_t pid , wait_pid;
    
    printf("Parent: Starting Input Validation...\n");
    
    if( validateArguments( argc ) || validateInput( argv , total )){
        
        printf("Parent: Will exit the program!\n\n");

        return FAILED ;
        
    }//Will stop any validation fails
    
    printf("Parent: Input Validation Passed!\n\n");
    
    printf("Parent: Requesting Memory...\n");

    shmID = shmget( IPC_PRIVATE ,  total * sizeof(int), IPC_CREAT | FLAG );
    
    if( shmID < PASS ){
        
        printf("Parent: Error in getting memory!!\n\n");
        
        return FAILED;
        
    }//If memory failure, print error and exit
    
    printf("Parent: Memory has been given succesfully!\n\n");

    
    printf("Parent: Attempting to attach memory...\n");

    
    shmPointer = shmat(shmID , NULL , 0);
    
    if( ( long )shmPointer == FAILED){
        
        printf("Parent: Error in attaching memory\n\n");
        
        return FAILED;
    }
    
    printf("Parent: Process has attached memory succesfully!\n\n");

    printf("Parent: Process will show memory!\n\n");

    displaySharedMemory( shmPointer , total , PARENT );
    
    printf("Parent: Process will fill memory from argsv!\n");

    fillMemory( shmPointer , argv , total );
    
    
    /* ACTIVATE BELOW TO TEST THE RETURN VALUE OF A CHILD WHEN THERE IS AN ERROR IN THE PROCESS */
    
    //shmPointer[1] = 100;
    
    for( count = 1 ; count <= total ; count++ ){
        
        pid = fork();
        
        if( pid >= PASS ){
            
            if(pid > PASS){
                
                printf("Parent (FORKING): Child %d forked succesfully!\n" , count);
                IDlist[count] = pid;

            }//This wil be done just by the Parent process.
            
            if( pid == PASS ){
            
                childProcess( count , total , shmPointer , pid );
            
            }//This is the child process
            
        }
        else {
            
            printf("Parent (FORKING): Error in forking Child %d\n\n" , count);
            exit(FAILED);
            
        }
        
    
    }//end of for loop that spawn children
    
    printf("Parent: Finished forking all children... \n\n");

    printf("Parent (WAITING): Starts the wait for its offspring to exit... \n\n");
        
    for ( count = 1 ; count <= total; count++ ){
        
        wait_pid = wait( &status );

        int Id = getUniqueID( wait_pid , total , IDlist );

        printf("Parent (WAITING): Waiting for child %d... \n\n" , Id );

        printf("Parent (WAITING): Child %d exit code was -%d-\n\n", Id , WEXITSTATUS( status ) );

        }
        
    printf("Parent: Waiting finished... All children are done \n\n");
    printf("Parent: Will print the shared memory space...\n\n");

    displaySharedMemory( shmPointer , total , PARENT );
        
    shmdt( (void *) shmPointer );
    printf("Parent: Program has detached memory!\n\n");
    shmctl(shmID , IPC_RMID ,  NULL);
    printf("Parent: Program has removed memory!\n\n");

    //This will be the parent process
    
    printf("Parent: Program has finished...\n\n");
 
    return 0;
    
}// end of main()

/**********************************************************************************	*/
/* validateNumber() receives a number and it checkes if it is a number between 
   MIN_NUMBER (set to 0) and MAX_NUMBER (set to 9). Both of these number can be 
   modified. It returns PASS (0) if it passes, otherwise it returns
   FAILED (-1) */
/**********************************************************************************	*/

int validateNumber( char number ){

    if( number < MIN_NUMBER || number > MAX_NUMBER ) {
        
        return FAILED;
    }
    
    return PASS;
    
}//end of validateNumber()


/**********************************************************************************	*/
/* validateArguments() will receive one int as a parameter, this is will be the
   number of arguments counted from the command-line. This function will return
   FAILED (-1) when the number of args from the command line does not fall within the
   set range. Otherwise it returns PASS (0)   */
/**********************************************************************************	*/

int validateArguments( int numberOfArgs){
    
    if( numberOfArgs < EXPECTED_ARGS_MIN || numberOfArgs > EXPECTED_ARGS_MAX ){
       
        displayArgError();
        return FAILED;
    }
    
    return PASS;
    
}//end of validateArguments()

/**********************************************************************************	*/
/* displaySharedMemory() receives an int pointer and 2 ints. One of the int varables 
   is to determine who called the function, a child or the parent. The other parameters
   are used to display the shared memory */
/**********************************************************************************	*/

void displaySharedMemory( int *memPointer , int total , int identity){
    
    int count;
    
    if ( identity == PARENT ){
    
    
        for( count = 0 ; count < total ; count++){
        
            printf("Parent: Shared Memory slot %d : [ %d ] \n" , count , memPointer[ count ]);
        
        }//prints memory slot from Parent
        
    }else if( identity ){
        
        for( count = 0 ; count < total ; count++){
            
            printf("Child %d: Shared Memory slot %d : [ %d ] \n" , identity , count , memPointer[ count ]);
            
        }//prints memory slot from Child
    }
    
    printf("\n");
    
}// end of displaySharedMemory()

/**********************************************************************************	*/
/* displayArgError() has no input. When called, it will display an error msg                                                       stating that the number of arguments was invalid and also shows the valid range */
/**********************************************************************************	*/


void displayArgError( void ){
   
    printf("\nParent: Not valid number of arguments... \n");
    printf("\tPlease make sure that the number of arguments \n");
    printf("\tis between %d and %d (not counting the command)\n\n" , EXPECTED_ARGS_MIN - 1,
           EXPECTED_ARGS_MAX - 1);
    
}//end of displayArgError()

/**********************************************************************************	*/
/* validateInput() gets the contents from argv as a pointer and an int with the total
   number of elements that are supposed to be acocunted for. It first checks if the 
   input is within the valid range. If it fails, it will display the error and where
   it occured (Position in shared memory). If it passes it will then check if the 
   number is not repeated. If it is, it will display an error message and will 
   where the error has occured. In both instances, it will stop the validation once
   an error has been found */
/**********************************************************************************	*/

int validateInput( char *argv[] , int total ){
    
    char holder;
    int count , result , temp , length;
    
    for( count = 1 ; count <= total; count++ ){
        
        holder = (*argv[ count ]);
        length = strlen( argv[count] );
        
        if( length > MAXLENGTH){
            
            printf("\nParent: Validation Failed!\n");
            printf("\tInvalid input at position %d in the command line.\n\n", count);
            
            return FAILED;
        }
        
        temp = atoi( argv[ count ] );
        
        result = validateNumber( (char)( temp + '0') );
        
        if(result == FAILED){
            
            printf("\nParent: Validation Failed!\n");
            printf("\tInvalid input at position %d in the command line.\n\n", count);
            
            return FAILED;
            
        }//this block checks if the input is within range
        
        int position;
        
        for( position = count ; position > 1 ; position--){
            
            temp = atoi( argv[ count ] );
            
            int runner = atoi( argv[ position - 1 ] );
            
            if( temp == runner ){
                
                printf("\nParent: Validation Failed!\n");
                printf("\tNumber repeats at position %d and position %d. \n" , ( position - 1 ) , count );
                printf("\tInput values have to be distinct. \n\n");

                
                return FAILED;
            }
            
        }// duplicate check
        
        
    }// this code validates and checks for duplicates
    
    return 0;
    
}//end of validateInput()

/**********************************************************************************	*/
/* fillMemory() receives an int pointer, and int variable and a char pointer
   array, these parameters will be used to fill up the shared memory poited by the
   int pointer and will fill it with the contents of argv */
/**********************************************************************************	*/


void fillMemory( int * memPointer , char * argv[] , int total ){
    
    int count;
    
    for(count = 0 ; count < total ; count++){
        
        memPointer[ count ] = atoi( argv[ count + 1 ] );
        
    }
    
}//end of fillMemory()

/**********************************************************************************	*/
/* getUniqueID() receives 2 ints and an int[], this look for the index in which the
   target process id is stored. It returns the index found else it returns FAILED (-1). The index corresponds to
   the appropiate child */
/**********************************************************************************	*/

int getUniqueID( int child_pid , int total , int list[] ){
    
    int count;
    int index = 0;
    
    
    for( count = 1 ; count <= total ; count++ ){
        
        if( list[ count ] == child_pid ){
            index = count;
            return index;
        }
        
    }
    
    return FAILED;
    
}

/**********************************************************************************	*/
/* childProcess() receives 2 ints and int pointer and a pid_t. This process will get the 
   contents of the corresponding shared memory location and will perform a multiplication 
   with its ID and store it back into the same memory location. If there is no errors, it will exit(0).
   Otherwise it will exit(-1)*/
/**********************************************************************************	*/

void childProcess( int count , int total , int *shmPointer , pid_t child_pid){

    int holder;
    int position = ( count - 1 ); //Position in the shared memory corresponding to a child n.
    
    child_pid = count;
    
    printf("\nChild %d: My Unique ID is -%d- and my Process ID is -%d-\n" , child_pid , child_pid, getpid() );
    printf("Child %d: Display shared memory state before process\n\n" , child_pid );
    
    displaySharedMemory( shmPointer , total , child_pid);
    
    holder = shmPointer[ position ];
    
    if( validateNumber( (char)(holder + '0') ) ){
        
        printf("Child %d: ERROR: MY MEMORY SPACE HAS AN INVALID INPUT!\n" , child_pid );
        printf("Child %d: This is my exit code -(%d)-\n\n" , child_pid, FAILED );
        
        exit( FAILED );
        
    }//This will check again in case the shared memory has been altered before child touched it
    
    shmPointer[ position ] = holder * child_pid;
    
    printf("Child %d will display shared memory state after process\n\n" , child_pid );
    
    printf("Child %d will display shared memory state after process\n\n" , child_pid );
    
    displaySharedMemory( shmPointer , total , child_pid );
        
    printf("Child %d: This is my exit code -%d-\n\n" , child_pid, PASS );
    
    exit( PASS );
    
}// end of childProcess()




