#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

#include "../config/config.h"

/*
 * !!! Do not edit below this line !!!
 * ...     Unless you are sure     ...
 */
#include "dbg.h"
#define PATH_SEPARATOR '/'

#define UP 1
#define DOWN 0

#define TRUE 1
#define FALSE 0

#define HOST_SIZE 255
#define BUF_LEN 1024

/* Defining time units */
#define _USEC 1
#define _MSEC 1000 * _USEC
#define _SEC 1000 * _MSEC

const char *prog_name;
char s_log_filename[225];
char s_out_filename[255];
FILE *fp_log;
FILE *fp_out;
char s_hostname[HOST_SIZE];
char s_hostname_short[HOST_SIZE];

int my_status;

/*
 * Set of all rechable servers
 *   status == DOWN => notrechable
 *          ==  UP  => rechable
 */
struct node {
	/* Node number = index+  1 */
	int number;
	/* Fully resolved hostname */
	char name[HOST_SIZE];
	/* Canonical Name */
	char c_name[HOST_SIZE];
	/* UP or DOWN */
	int status;
} serv_list[TOTAL_SERVERS];

/*
 * Queues
 */
typedef struct queue_struct {
	char host[HOST_SIZE];
	char data[BUF_LEN];
	struct queue_struct *next;
} queue;

queue *tcp_q;
queue *udp_q;

/*
 * Thread locks
 */
pthread_mutex_t lock_ping_status;
pthread_mutex_t lock_tcp_q;
pthread_mutex_t lock_udp_q;
pthread_mutex_t lock_file;

/*
 * Function decleration for common functions
 */
void diep ( char * );
char * resolve_hostname ( char * );
int get_serv_index ( char * );
void parse_config ( void );
void print_con_list ( void );
const char * get_program_name ( char * );

/* Queue function declerations */
void insert_queue ( queue **, char *, char * );
queue * remove_queue ( queue ** );
int is_queue_empty ( queue * );
void print_queue ( queue * );
