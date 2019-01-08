// Nicholas Tofani
// PROGRAMMING ASSIGNMENT 3 - Ordered Ready Queue
// 9/20/2017
// Regular compile -> run "cc asgn3-tofani2.c"
// Run: -> "./a.out"
/* 
   An ordered doubly-linked ready queue based on Processes's priority that
    stores all processes ready for execution in an operating system.  This program
    (1) accepts a char from keyboard;
         if(char == +)*input data format: id, prog, type, priority, arrivalTime, runTime*
            grab process info
            appends the process to the correct location of the doubly-linked ready queue;
         if(char == ?) *input data format: id*
            Query's queue for process based on processID entered
         if (char == -) *input data format: id*
            find and deleted process base on processID entered
         if (char == #)
            end program
    (2) Repeat step 1 until "#" is typed by the user,
    (3) after "#" is read, prints all the process records twice, one
        from the beginning to the end and then in the reverse order.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//First node in Queue
struct HEADER {
    struct PROCESS *headptr;
    struct PROCESS *tailptr;
    int count;
};

void initHEADER(struct HEADER *newHeader) {
    newHeader->headptr = NULL;
    newHeader->tailptr = NULL;
    newHeader->count = 0;
}

//Each Process in a node in Queue
struct PROCESS {
    int id;              // domain = { positive integer }
    char program[25];    // domain = { [a-zA-Z][a-zA-Z0-9_\.]* }
    int jobType;         // domain = { 0, 1, 2, 3 }
    int priority;        // domain = { 1 .. 8 }
    long arrivalTime;    // domain = { non-negative long }
    int execTime;        // domain = { 1 .. 20 }
    struct PROCESS *prev;
    struct PROCESS *next;
};

void initPROCESS(struct PROCESS *newProcess) {
    newProcess->prev = NULL;
    newProcess->next = NULL;
    //setting to negative to make sure legit info is submitted from user
    newProcess->execTime = -1;
}

//Declaring function prototypes
void dumpQueue(struct HEADER *header);
void append(struct HEADER *header, struct PROCESS *newProcess);
struct PROCESS queryQueue(struct HEADER *header, int processID);
void deleteProcess(struct HEADER *header, int beingDeletedID);

int main(int argc, const char * argv[]) {
    time_t currentTime;
    struct HEADER header;
    struct HEADER *headerptr = &header;
    initHEADER(headerptr);
    struct PROCESS *newProcptr;
    int requestedExit = 0;//Treat as Boolean
    
    //requestedExit will only change if "#" is typed by user
    while(requestedExit == 0) {
        char firstSymbolEntered[2];
        int programID;
        
        printf("Enter a valid command and its relevant parameters \n");
        scanf("%s", &firstSymbolEntered[0]);
        
        //Uses the first Char entered by user to decide what operation to do
        switch (firstSymbolEntered[0]) {
            
            case '+': //Add process to Queue
                newProcptr = malloc(sizeof(struct PROCESS));
                initPROCESS(newProcptr);
                //Grabbing input from user for new Process
                scanf("%d %s %d %d %d", &newProcptr->id, newProcptr->program,
                       &newProcptr->jobType, &newProcptr->priority,
                       &newProcptr->execTime);
                newProcptr->arrivalTime = currentTime;
                append(headerptr, newProcptr);
                printf("ADD: %i, %s, %i, %i, %ld, %i \n", newProcptr->id, newProcptr->program,
                       newProcptr->jobType, newProcptr->priority, newProcptr->arrivalTime,
                       newProcptr->execTime);
            break;
                
            case '?': //Query process by ID
                //Grabbing programID input from user to use to query
                scanf("%d", &programID);
                
                //Making sure the list isn't empty
                if(headerptr->headptr != NULL) {
                    struct PROCESS foundProcess = queryQueue(headerptr, programID);
                    struct PROCESS *foundProcessPtr = &foundProcess;
                    if(foundProcessPtr->execTime != -1) {
                        printf("QUERY: %i, %s, %i, %i, %ld, %i \n", foundProcess.id,
                               foundProcess.program, foundProcess.jobType, foundProcess.priority,
                               foundProcess.arrivalTime, foundProcess.execTime);
                    }
                    else {
                        free(foundProcessPtr);
                        printf("ERROR - Process does not exist. \n");
                    }
                }
                else {
                    printf("Can't be found because the List is empty. \n");
                }
                
            break;
                
            case '-': //Delete Process by ID
                scanf("%d", &programID);
                if(headerptr->headptr != NULL) {
                    deleteProcess(headerptr, programID);
                }
                else {
                    printf("List is empty so program can't be deleted! \n");
                }
            break;
                
            case '#': //Stops the Program
                printf("TERMINATE: \n");
                //Make sure theres something to dump
                if(headerptr->headptr != NULL) {
                    dumpQueue(headerptr);
                }
                else {
                    printf("No process in ready queue. \n");
                }
                printf("END \n");
                requestedExit = 1;
            break;
            
            default: //Prints if a nonvalid command is entered
                printf("Command not recognized \n");
            break;
        }
    } 

}

// Appends new node to in ordered Queue by priority
void append(struct HEADER *header, struct PROCESS *newProcess) {
    //Makes sure the ID hasn't been used before
    if(queryQueue(header, newProcess->id).execTime != -1) {
        printf("ERROR - Process id exists - %i \n",  newProcess->id);
        return;
    }
    
    //Checks if its adding a node that will be first & head & tail of Queue
    if(header->headptr != NULL) {
        //Not first Process to be Added
        //Checking where newProcess should be added
        if(newProcess->priority > header->headptr->priority){
            struct PROCESS *oldHead = header->headptr;
            header->headptr = newProcess;
            newProcess->next = oldHead;
            oldHead->prev = newProcess;
        }
        else if(newProcess->priority < header->tailptr->priority) {
            struct PROCESS *oldTail = header->tailptr;
            header->tailptr = newProcess;
            newProcess->prev = oldTail;
            oldTail->next = newProcess;
        }
        else {
            //Not head or Tail... Just a regular sorted Append
            struct PROCESS *currentSpot = header->headptr->next;
            
            //Finding spot for new process
            while(currentSpot->priority > newProcess->priority &&
                  currentSpot->next != NULL) {
                currentSpot = currentSpot->next;
            }
            
            currentSpot->prev->next = newProcess;
            newProcess->prev = currentSpot->prev;
            currentSpot->prev = newProcess;
            newProcess->next = currentSpot;
        }
    }
    else {
        //Is the first element in Queue ++ Head of Queue ++ tail of Queue
        header->headptr = newProcess;
        header->tailptr = newProcess;
    }
    
    //Queue has grown by one
    header->count = header->count + 1;
}

/*
  Finds and returns PROCESS that has same ID as processID
   Returns newly initiated Process if Process doesn't exist
   if "returned Value".execTime == -1... Process *NOT FOUND*
                                         User will be Alerted
*/
struct PROCESS queryQueue(struct HEADER *header, int processID) {
    int wasFound = 0; //treat as boolean
    struct PROCESS *currentProcess = header->headptr;
    
    //Checking each PROCESS in Queue and comparing ID to processID
    for(int i = 0; i < header->count && wasFound == 0; i++) {
        if(currentProcess->id == processID) {
            wasFound = 1;
        }
        else {
            currentProcess = currentProcess->next;
        }
    }
    
    //Checking if the processID was found
    if(wasFound == 0) {
        //Wasn't Found
        //Avoiding returning NULL error will free after return
        currentProcess = malloc(sizeof(struct PROCESS));
        initPROCESS(currentProcess);
    }
    return *currentProcess;
}

/* 
  Finds and deletes the Process associated with beingDeletedID,
   Then Relinks Queue without the deleted Process
   If Process cannot be found, User is alerted
*/
void deleteProcess(struct HEADER *header, int beingDeletedID) {
    //Getting Memory location of process that will be deleted
    struct PROCESS processToDelete = queryQueue(header, beingDeletedID);
    struct PROCESS *processToDeletePtr = &processToDelete;
    
    if(processToDelete.execTime == -1) {
        //Process Not Found... Exit Function! after freeing Memory
        free(processToDeletePtr);
        printf("ERROR - Process does not exist. \n");
        return;
    }
    
    //Saving next&previous ptr to be reassigned
    struct PROCESS *deletedsNext = processToDeletePtr->next;
    struct PROCESS *deletedsPrevious = processToDeletePtr->prev;
    
    //Dumping Process Info before Deleting
    printf("DELETE: %i, %s, %i, %i, %ld, %i \n", processToDeletePtr->id, processToDeletePtr->program,
            processToDeletePtr->jobType, processToDeletePtr->priority, processToDeletePtr->arrivalTime,
            processToDeletePtr->execTime);
    
    //Re-linking Queue
    if(processToDeletePtr->id == header->headptr->id) {
        //is Head
        header->headptr = deletedsNext;        
    }
    else if (processToDeletePtr->id == header->tailptr->id) {
        //is tail
        header->tailptr = deletedsPrevious;
    }
    else {
        //Not head or tail
        deletedsPrevious->next = deletedsNext;
        deletedsNext->prev = deletedsPrevious;
    }
    free(processToDeletePtr);
    
    //Queue has shrunk by 1
    header->count =  header->count - 1;
}

//Dumps Queue in Ascending Order && Descending Order using header's head & tail
void dumpQueue(struct HEADER *header) {
    //Ascending Order dumping
    struct PROCESS *currentSpot = header->tailptr;
    printf("Ascending Order: \n");
    for(int i = 0; i < header->count; i++) {
        printf("%i, %s, %i, %i, %ld, %i \n", currentSpot->id, currentSpot->program,
                currentSpot->jobType, currentSpot->priority, currentSpot->arrivalTime,
                currentSpot->execTime);
        currentSpot = currentSpot->prev;
    }
    
    //Descending Order dumping
    currentSpot = header->headptr;
    printf("Descending Order: \n");
    for(int j = 0; j < header->count; j++) {
        printf("%i, %s, %i, %i, %ld, %i \n", currentSpot->id, currentSpot->program,
                currentSpot->jobType, currentSpot->priority, currentSpot->arrivalTime,
                currentSpot->execTime);
        currentSpot = currentSpot->next;
    }
}
