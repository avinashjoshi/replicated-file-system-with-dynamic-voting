#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Edit the vaues below this line */
#define PORT 2662
#define CONF_FILE "config/file_servers.conf"
#define TOTAL_SERVERS 7

/* Uncomment to *disable* DEBUG messages */
//#define NDEBUG
/* Uncomment to *enable* TEST sections */
#define _TEST_

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

const char *prog_name;
char s_log_filename[225];
FILE *fp_log;
char s_hostname[HOST_SIZE];
char s_hostname_short[HOST_SIZE];

struct node {
	int number;
	char name[HOST_SIZE];
	int status;
} serv_list[TOTAL_SERVERS];
