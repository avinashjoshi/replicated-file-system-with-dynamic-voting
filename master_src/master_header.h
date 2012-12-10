#include "../common/common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

pthread_t tcp_thread;
pthread_t lock_tcp_sock;
int sock_tcp[TOTAL_SERVERS];

int tcp_replies;

/*
 * All function declerations here
 */

void tcp_send_init ( void );
void * handle_tcp ( void * );
void close_tcp_socks ( void );
void handle_servers ( void );
