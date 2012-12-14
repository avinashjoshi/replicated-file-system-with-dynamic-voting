#include "../common/common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define READ 0
#define WRITE 1

//Main UDP/TCP Thread
pthread_t udp_thread;
pthread_t tcp_thread;
//Thread that checks TCP/UDP queues
//and does necessary actions
pthread_t tcp_queue_thread;
pthread_t udp_queue_thread;
pthread_t udp_timer;

pthread_mutex_t lock_status;
pthread_mutex_t lock_p;

//sockets for TCP and UDP
int sock_udp_send, sock_udp_recv;
int sock_tcp;


int my_status;
int my_id;

int recv_votes;
int total_voters;

int is_voting;
pthread_mutex_t lock_voting;
pthread_mutex_t lock_p;

int P[TOTAL_SERVERS];
int T[TOTAL_SERVERS];
int M;
char I[TOTAL_SERVERS + 1];

struct vote_response {
	int status;
	int vn;
	int ru;
	char ds[TOTAL_SERVERS];
} votes[TOTAL_SERVERS];

/* For the main algo */
int vn;
int ru;
char ds[TOTAL_SERVERS + 1];

/*
 * All function declerations here
 */

void udp_recv_init ( int );
void udp_send_init ( void );
void udp_send ( char *, int, char * );
void *handle_udp ( void * );
void *handle_udp_queue ( void * );

void ping ( int );
void print_rechable_servers ( void );
void ping_servers ( void );
void reset_timers ( void );
void reset_servers ( void );
void *handle_timer ( void * );

void tcp_recv_init ( int );
void *handle_tcp ( void * );
void *handle_tcp_queue ( void * );

/* To do with voting */
void send_vote_request ( void );
void reset_votes ( void );
int is_distinguished ( void );
int belongs_to ( char *, char * );
int is_latest_copy ( void );
int vote_request_rw ( int );
int total_voted_servers ( void );

void test_udp_send ( int );
