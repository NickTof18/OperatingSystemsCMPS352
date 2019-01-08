// Nicholas Tofani
// PROGRAMMING ASSIGNMENT 4 - Simple Shell
// 10/2/2017
// Regular compile -> run "cc asgn4-tofanin2.c"
// Run: -> "./a.out"

/*
  This program uses the fork, execvp, waitpid, chdir, and other
  process management system calls to create a program that acts
  like a shell on the FreeBSD operating system.  This Program is
  operating system dependent.  Upon execution the user is welcomed
  then can run other processes as children of the Shell.
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int readLine(char *instring);
int isWhiteSpace(char c);
int startProcess(int numOfArgs, char *args[10]);
int needsToBeRunOnParent(char *process[10]);

int main() {
    //Welcomes user with his/her username
    printf("%s: \n", getlogin());
    int continueRunning = 1;//treat as bool

    do {
        printf("Please enter a valid command. \n");
        int numOfCommands = 0;
        int length = 0;
        char *tmp;
        char instring[80];             // store one line of input
        char *argv[10];              // store parameters for execv()
        length = readLine(instring);
        
        tmp = instring ;
        argv[numOfCommands] = strsep(&tmp, " \t"); // put first word int argv[0]
        while ((numOfCommands < 10) && (argv[numOfCommands] != '\0'))
        {
            numOfCommands++;
            argv[numOfCommands] = strsep(&tmp, " \t");
        }
        
        continueRunning = startProcess((numOfCommands - 1), argv);

        //shouldn't need to use the while loop condition
        //  but if exit fails this is backup
    } while(continueRunning != 0);

    return 0;
}

/*
  Depending on the results of needsToBeRunOnParent...
  This function will fork and excute the command passed
  to it in args[0] and wait for the process to finish
  excution.. if args[0] needs to run on parent
  needsToBeRunOnParent will run it and start process will
  do nothing but return 1
      Returns 0 if exit requested and failed
      Returns 1 if needs parent
      Returns -1 if it can be run on child
*/
int startProcess(int numOfArgs, char *args[10]) {
    int status;
    int c_pid;
    //1=parent, -1=child, 0=exit requested
    int needsParent = needsToBeRunOnParent(args);
    if(needsParent == -1) {
        //Attempts to fork
        if ((c_pid = fork()) == 0) {
            //is a child process
            if (execvp(args[0], args) == -1) {
                //execvp FAILED dump output
                printf("Error: ");
                //Printing All arguments
                for (int i = 0; i <= numOfArgs; i++) {
                    printf(" %s ", args[i]);
                }
                //Needs to move to next line after everything is printed
                printf("\n");
            }
            //kill child after execvp fails and dumped input
            exit(0);
        }
        else if (c_pid > 0) {
            //is a Parent process && waiting for child
            c_pid = waitpid(c_pid, &status, 0);
        }
        else {
            printf("fork failed!\n");
        }
    }
    return needsParent;
}

/*
   Compares user typed program named to a list of commands
   that require to be run on the parent
   **CURRENTLY ONLY WORKS WITH
    *exit
    *cd
    *chdir
   **
      Returns 0 if exited requested
      Returns 1 if needs parent
      Returns -1 if it can be run on child
*/
int needsToBeRunOnParent(char *process[10]) {
    if(strcmp(process[0], "exit") == 0) {
        exit(0);
        //if exit fails still kills loop
        return 0;
    }
    else if(strcmp(process[0], "cd") == 0 || strcmp(process[0], "chdir") == 0){
        chdir(process[1]);
        return 1;
    }
    return -1;
}

int isWhiteSpace(char c) {
    int i = 0;
    char whiteSpaces[] = {' ', '\t'};
    while (whiteSpaces[i] != '\0') {
        if (c == whiteSpaces[i++])
            return 1;
    }
    return 0;
}

int readLine(char *instring) {
    // read a char, skip extra white spaces, put it in instring[]
    // and put a '\0' at the end
    int i = 0;
    int whiteSpace = 0;
    instring[i] = getc(stdin);     // stdin is the keyboard
    while (instring[i] != '\n') {
        if (whiteSpace) {
            if (!isWhiteSpace(instring[i])) {
                i++;
                whiteSpace = 0;
            }
        } else {
            if (isWhiteSpace(instring[i])) {
                whiteSpace = 1;
            }
            i++;
        }
        instring[i] = getc(stdin);
    }
    // if there is a white space before carry return
    // cut it out.
    if (isWhiteSpace(instring[i-1])) {
        i--;
    }
    instring[i] = '\0';  // replace '\n' with '\0'
    
    return i + 1;
}
