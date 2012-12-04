#include "../config/common.h"

pthread_t udp_thread;

int sock_udp_send, sock_udp_recv;

/*
 * All function declerations here
 */

void parse_config ( void );
void print_con_list ( void );
const char * get_program_name ( char * );
void udp_recv_init ( int );
void udp_send_init ( void );
void udp_send ( char *, int, char * );
void diep ( char * );
void *handle_udp ( void * );
void ping_servers ( void );
char * resolve_hostname ( char * );

void test_udp_send ( int );
