/*
reference: ps6 codes
*/

#include "part_c.h"


#include <ctype.h>

void
my_prog_1(char* host, char* blackbox, char* file_path)
{
    CLIENT *clnt;
    char * *result_1;
    numbers  add_1_arg;
    FILE *file;
    file = fopen(file_path, "a");
    add_1_arg.path = blackbox; //assign path of blackbox to arg.path for sending server

#ifndef    DEBUG
    clnt = clnt_create (host, MY_PROG, MY_VERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror (host);
        exit (1);
    }
#endif    /* DEBUG */
    int input1, input2;
    scanf("%d %d", &input1, &input2); //taking inputs from stdin

    add_1_arg.a = input1; //assign input1 to arg.a for sending server
    add_1_arg.b = input2; //assign input2 to arp.p for sending server


    result_1 = add_1(&add_1_arg, clnt); //to take the result of the blackbox from server

    if (result_1 == (char **) NULL) {
        clnt_perror (clnt, "call failed");
    }
        if(isdigit(*result_1[0]) || *result_1[0] == '-'){ //case of blacbox gives integer output

            fprintf(file, "SUCCESS:\n");
        }
        else{ //case of blackbox does not give integer output
            fprintf(file, "FAIL:\n");
        }
       
        fprintf(file, "%s",*result_1); //writing the output of blacbox to part_c_output.txt file

    fclose(file);
#ifndef    DEBUG
    clnt_destroy (clnt);
#endif     /* DEBUG */
}


int
main (int argc, char *argv[])
{
    char *host;
    char* blackbox;
    char* file_path;
    
    host = argv[3];
    blackbox = argv[1];
    file_path = argv[2];
    my_prog_1 (host, blackbox, file_path);
exit (0);
}
