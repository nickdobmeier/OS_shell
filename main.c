// Nicholas Dobmeier  -  njd170130

#include <stdio.h>              // necessary header files
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#define SIZE 90                 // max length of each user command
void childProcess(char* str);   // prototypes
void err_sys(const char* x);
void removeLast(char* str, int n);


int main(int argc, char* archv[])
{
    char inputCommand [SIZE];   // create buffer to hold user command input
    int numCommands = 0;
    
    do{
        printf("Shell - enter command\t$ ");
        numCommands++;
                                                            // last character of input string is always a NEWLINE character
        fgets(inputCommand , SIZE , stdin);                 // 90 specifices max length (including '\n' so really max length is 89)
        if(strcmp(inputCommand, "exit\n") == 0){            // terminate when user types "exit" and presses [enter]
            break;
        }
        int cLength = (int)strlen(inputCommand);
        bool toWait = true;
        if( (cLength > 1) && inputCommand[cLength-2] == '&')        // check if 2nd to last (before '\n') char is '&'
        {
            removeLast(inputCommand, cLength);                      // remove '&' character from end of command
            toWait = false;
        }                                               // delimiter will remove newline character in either case
        
        int pid = fork();
        if (pid < 0) { err_sys("\t**Fork Failed**"); }  // if error forking
        
        else if(pid == 0){      // CHILD code
            //printf("The child and parent process IDs are:  %d   &   %d   respectively\n", (int)getpid(), (int)getppid()); //*****
            childProcess(inputCommand);
            exit(0);                                    // child process ends properly
            
        }else{                  // PARENT code
            if(toWait == true){
                int status = 0;
                int childPid = waitpid(pid, &status, 0);                                    // parent sleeps while child runs
                    // wait for the SPECIFIC child called and NOT just an arbitrary child
                    // case where a new child is created before the old command with "&" appended completes...**
                // waitpid returns pid of the child when terminated properly
            }else{
                printf("\tChild process %d is executing the command.\n", pid);
            }
        }
    }while(1);                                          // only exit loop upon the break statement above
    printf("\nNumber of commands entered = %d\n", numCommands);
}



void childProcess(char* inputCommand){
    int num_Tokens_Allowed = 20;
    char delimiterChars[3] = {32, 9, 10};                   // [space], [tab], '\n' delimeter chars
    
    char** strTokens;
    strTokens = malloc(num_Tokens_Allowed * sizeof(char*)); // max of 19 tokens allowed per command (+1 for NULL)
    
    char* token = strtok(inputCommand, delimiterChars);     // grab first token (or NULL)
    int numTokens = 0;
    while (token != NULL)
    {
        if(numTokens >= num_Tokens_Allowed-1 )      // need room for a NULL index at the end, so need to have atleast one spot allocated for that
        {
            num_Tokens_Allowed *= 2;                // for realloc(), double number of allowed tokens
            strTokens = realloc(strTokens, num_Tokens_Allowed * sizeof(char*));
        }
        strTokens[numTokens] = malloc((SIZE+1) * sizeof(char));     // create space for each new token found
        strcpy(strTokens[numTokens], token);                        // copy token into newly created space
        
        numTokens++;
        token = strtok(NULL, delimiterChars);       // grab next token
    }                                               // In subsequent calls, the function expects a null pointer and uses the position right after
                                                            // the end of the last token as the new starting location for scanning.
    strTokens[numTokens] = NULL;                    // last index NULL so that exec() knows where last argument in strTokens is
    
    if (execvp(strTokens[0], strTokens) < 0) {      // don't need    ( strTokens[0] == NULL || )
        printf(" *FAILED to execute command*\n");   // error message when command fails
    }
    
    for (int i = 0; i < numTokens; i++){            // free up space in 2nd dimension
        free(strTokens[i]);}
    free(strTokens);                                // free up space in first dimension
}


void removeLast(char* str, int n)           // pointer parameter, so we are modifying the original argument passed as well
{
    str[n-2]=0;                             // delete '&', replace with null terminator
}                                           // delimiter will talke care of deleting '\n' character



void err_sys(const char* x)
{
    perror(x);                              // output parameter 'x' followed by error message
    exit(1);                                // terminate process will error signal
}
