all: part_c_client.out	part_c_server.out part_c_logger.out

part_c_client.out:	part_c.h 	part_c_client.c		part_c_clnt.c	part_c_xdr.c
	gcc part_c_client.c	part_c_clnt.c	part_c_xdr.c	-o 	part_c_client.out

part_c_server.out:	part_c.h	part_c_server.c		part_c_svc.c		part_c_xdr.c
	gcc 		part_c_server.c		part_c_svc.c	part_c_xdr.c	-o 	part_c_server.out
part_c_logger.out:
	gcc	part_c_logger.c	-o part_c_logger.out
