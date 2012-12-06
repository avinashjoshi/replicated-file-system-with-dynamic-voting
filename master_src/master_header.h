#include "../config/common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

pthread_t tcp_thread;
int sock_tcp[TOTAL_SERVERS];

/*
 * All function declerations here
 */

void tcp_send_init ( void );
void * handle_tcp ( void * );
