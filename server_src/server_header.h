#include "../common/common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

//Main UDP/TCP Thread
pthread_t udp_thread;
pthread_t tcp_thread;
//Thread that checks TCP/UDP queues
//and does necessary actions
pthread_t tcp_queue_thread;
pthread_t udp_queue_thread;

//sockets for TCP and UDP
int sock_udp_send, sock_udp_recv;
int sock_tcp;

/*
 * All function declerations here
 */

void udp_recv_init ( int );
void udp_send_init ( void );
void udp_send ( char *, int, char * );
void *handle_udp ( void * );
void *handle_udp_queue ( void * );
void ping_servers ( void );

void tcp_recv_init ( int );
void *handle_tcp ( void * );
void *handle_tcp_queue ( void * );

void test_udp_send ( int );
