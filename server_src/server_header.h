#include "../config/common.h"

pthread_t udp_thread;

/*
 * All function declerations here
 */

void parse_config ( void );
void print_con_list ( void );
const char * get_program_name ( char * );
void udp_init ( int );
void diep ( char * );
void *handle_udp ( void * );

void test_udp_send ( int );
