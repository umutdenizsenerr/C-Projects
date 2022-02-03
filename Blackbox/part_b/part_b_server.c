/*
referenced from ps6
 */

#include "part_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

char **
add_1_svc(numbers *argp, struct svc_req *rqstp)
{
    int     f1[2], f2[2];
    static char * result;
    pid_t   process_id;
    char    blackbox_input[10000]; //creating char array to give blackbox as input
    char blackbox_output[10000]; //creating char array to take from blackbox as output

    pipe(f1); //creating pipe parent to child
    pipe(f2); //creating pipe child to parent

   
    
    process_id = fork(); //creating child process

    if(process_id == -1) {
        fprintf(stderr, "fork() failed.\n"); //if there is a problem with creating the child process it will print error message
        exit(-1);
    }

    if(process_id == 0) { //child process
        dup2(f1[0], STDIN_FILENO); // stdin of child process reads from f1 pipe
        dup2(f2[1], STDOUT_FILENO); // child process writes it's stdout to f2 pipe
        dup2(f2[1], STDERR_FILENO); // child process writes it's stederr to f2 pipe
        close(f2[0]); // closing read part of f2 pipe
        close(f2[1]); // closing write part of f2 pipe
        close(f1[0]); // closing read part of f1 pipe
        close(f1[1]); //closing  write part of f1 pipe

        execl(argp->path, argp->path, NULL); //child process executes blackbox


    }
    else { //parent process
        
        close(f1[0]); //parent will not read from f1 pipe
        close(f2[1]); //parent will not write to f2 pipe

        memset(blackbox_output, 0, sizeof blackbox_output); //clear blackbox_output for the next execution of program

        sprintf(blackbox_input, "%d %d\n", argp->a, argp->b); //convert arguments from main program to integer

        write(f1[1], blackbox_input, strlen(blackbox_input)+1); //writes inputs of blackbox to f1 pipe

        read(f2[0], blackbox_output, sizeof(blackbox_output)+1); //reads output of blackbox from f2 pipe

    }
    result = blackbox_output;

	return &(result);
}
