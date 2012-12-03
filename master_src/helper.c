/*
 * File: helper.c
 *
 * Containes all helper functions
 * that may be necessary for this application
 *
 * Author: Avinash Joshi <avinash.joshi@utdallas.edu>
 */

#include "server_header.h"
#include <netdb.h>
#include <netinet/in.h>

/*
 * get_program_name takes string as input
 * and gets the fp_config (executable) name
 * from the last '/' character
 */
const char*
get_program_name ( char argv[] ) {

	/* strrchr gives the last occurance of PATH_SEPARATOR in argv[0] */
	const char *exec_name;

	exec_name = strrchr (argv, PATH_SEPARATOR);
	if (!exec_name)
		exec_name = argv;
	else
		++exec_name;

	return exec_name;
}

/*
 * parse server config fp_config...
 */
void
parse_config ( void ) {
	struct hostent *he;
	int i_serv = 0;
	FILE *fp_config;
	char *c;
	char s_line[HOST_SIZE];

	fp_config = fopen ( CONF_FILE, "r" );
	ASSERT ( fp_config, " - %s", CONF_FILE );

	bzero ( s_line, HOST_SIZE );

	while ( fgets ( s_line, sizeof s_line, fp_config) != NULL ) {// Read line
		// Stop reading file if there are more than the specified total
		if ( i_serv == TOTAL_SERVERS ) {
			break;
		}
		c = strchr ( s_line, '\n' );
		//Remove trailing \n
		if (c) {
			*c = 0;
		}
		he = gethostbyname ( s_line );
		debug_clean ("%d: %s => %s", i_serv + 1, s_line, he->h_name);
		// Insert details into serv_list structure
		serv_list[i_serv].number = i_serv + 1;
		strcpy ( serv_list[i_serv].name, he->h_name );
		serv_list[i_serv].status = DOWN;

		bzero ( s_line, HOST_SIZE );
		i_serv ++;
	}
	ASSERT ( (i_serv >= TOTAL_SERVERS), " - Looks like you have less than %d hosts listed in %s", TOTAL_SERVERS, CONF_FILE );
	return;
}

/*
 * Print connection list
 */
void
print_con_list ( void ) {
	int i_serv;
	printf ( "CONN LIST: \n");
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		printf ( "%d: %s (Status: %d)\n", serv_list[i_serv].number, serv_list[i_serv].name, serv_list[i_serv].status );
	}
	return;
}

void diep(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}
