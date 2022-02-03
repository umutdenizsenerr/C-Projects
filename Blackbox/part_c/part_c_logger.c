/*
reference: ps6 codes, https://www.geeksforgeeks.org/socket-programming-cc/
*/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int logger_socket, data_socket;
    int opt = 1;
    struct sockaddr_in logger_address;
    int length = sizeof(logger_address);

    char output[10000];
    char *log_path = argv[1];
    int port = atoi(argv[2]);

    logger_socket= socket(AF_INET, SOCK_STREAM, 0); //creating socket for logger
    setsockopt(logger_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)); //helps to reuse address and port in each execution

    logger_address.sin_family = AF_INET;
    logger_address.sin_addr.s_addr = INADDR_ANY; //assign server ip address to local host
    logger_address.sin_port = htons( port ); //assign port number
    
    bind(logger_socket, (struct sockaddr *)&logger_address,length);  //binding the server to the server ip address and port number
    
    listen(logger_socket, 3); //waiting for part_c_server.c to send data

    data_socket = accept(logger_socket, (struct sockaddr *)&logger_address, (socklen_t*)&length); //accept data that are coming from part_c_server.c

    read(data_socket , output, 10000); //read data and write it to string


    FILE *file;
    file = fopen(log_path, "a");
    fprintf(file, "%s", output); //writing to file
    fclose(file);
    return 0;
}
