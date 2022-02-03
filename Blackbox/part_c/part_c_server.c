/*
reference: ps6 codes, https://www.geeksforgeeks.org/socket-programming-cc/
*/
#include "part_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>


char **
add_1_svc(numbers *argp, struct svc_req *rqstp)
{
    extern char *logger_ip; //take server ip address from global variable
    extern int portno;
    int     f1[2], f2[2];
    static char * result;
    pid_t   process_id;
    char    blackbox_input[10000]; //creating char array to give blackbox as input
    char blackbox_output[10000]; //creating char array to take from blackbox as output
    char logger_buffer[10000];
    pipe(f1); //creating pipe parent to child
    pipe(f2); //creating pipe child to parent

    int socket_id;
    struct sockaddr_in serv_addr;

    socket_id = socket(AF_INET, SOCK_STREAM, 0); //creating socket
 
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); //assigning port number to the socket
       
    char *server_id;

    if(strcmp(logger_ip, "localhost")==0){
        server_id = "127.0.0.1"; //if we use local host we need to assign 127.0.0.1 as server ip address
    }
    else{
        server_id = logger_ip; //else we take directly server ip address
    }

    inet_pton(AF_INET, server_id, &serv_addr.sin_addr); //giving server_id as server ip address


   
    connect(socket_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); //connecting socket with the port

    

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

        sprintf(blackbox_input, "%d %d\n", argp->a, argp->b); //convert arguments from main program to string

        write(f1[1], blackbox_input, strlen(blackbox_input)+1); //writes inputs of blackbox to f1 pipe

        read(f2[0], blackbox_output, sizeof(blackbox_output)+1); //reads output of blackbox from f2 pipe

        sprintf(logger_buffer, "%d %d %s", argp->a, argp->b, ((isdigit(blackbox_output[0]) || blackbox_output[0] == '-')? blackbox_output: "_\n")); // convert arguments of main program to string
        send(socket_id , logger_buffer , strlen(logger_buffer) , 0 ); //to send logger buffer to the logger.c program

    }


    result = blackbox_output;
    return &(result);
}
