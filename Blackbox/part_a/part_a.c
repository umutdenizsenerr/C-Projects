#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

void myprog(char* blackbox, char *filepath){
    int     f1[2], f2[2];
    pid_t   process_id;
    char    blackbox_input[10000]; //creating char array to give blackbox as input
    char blackbox_output[10000]; //creating char array to take from blackbox as output
    int input1, input2;
    scanf("%d %d", &input1, &input2); //taking required inputs

    pipe(f1); //creating pipe parent to child
    pipe(f2); //creating pipe child to parent

    FILE *file;
    file = fopen(filepath, "a"); //to append end of the file
    
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

        execl(blackbox, blackbox, NULL); //child process executes blackbox


    }
    else { //parent process
        
        close(f1[0]); //parent will not read from f1 pipe
        close(f2[1]); //parent will not write to f2 pipe

        sprintf(blackbox_input, "%d %d\n", input1, input2); //convert input1 and input2 to integer

        write(f1[1], blackbox_input, strlen(blackbox_input)+1); //writes inputs of blackbox to f1 pipe

        wait(NULL);

        read(f2[0], blackbox_output, sizeof(blackbox_output)+1); //reads output of blackbox from f2 pipe

        if(isdigit(blackbox_output[0]) || blackbox_output[0] == '-'){ //case of blacbox gives integer output

            fprintf(file, "SUCCESS:\n");
        }
        else{ //case of blackbox does not give integer output
            fprintf(file, "FAIL:\n");
        }
        fprintf(file, "%s",blackbox_output); //writing the output of blacbox to part_a_output.txt file
       
    }

    fclose(file);
}

int
main (int argc, char *argv[])
{
    char *blackbox;
    char *filepath;

    blackbox = argv[1];
    filepath = argv[2];
   myprog(blackbox, filepath);

    return(0);
}
