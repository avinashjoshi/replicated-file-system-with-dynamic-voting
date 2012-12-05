#include "../config/common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

pthread_t tcp_thread;
int sock_tcp[TOTAL_SERVERS];

/*
 * All function declerations here
 */

void parse_config ( void );
void print_con_list ( void );
const char * get_program_name ( char * );
void tcp_send_init ( int );
void diep ( char * );
void * handle_tcp ( void * );
char * resolve_hostname ( char * );
