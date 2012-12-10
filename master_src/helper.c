/*
 * File: helper.c
 *
 * Containes all helper functions
 * that may be necessary for this application
 *
 * Author: Avinash Joshi <avinash.joshi@utdallas.edu>
 */

#include "master_header.h"

void
close_tcp_socks ( void ) {
	int i_sock;
	for ( i_sock = 0; i_sock < TOTAL_SERVERS; i_sock++ ) {
		close ( sock_tcp[i_sock] );
	}
}

void
handle_servers ( void ) {
	FILE *fp_commands;
	char s_line[BUF_LEN];

	char *token;
	char s_buffer[BUF_LEN];
	char command[BUF_LEN];
	int value;
	char message[BUF_LEN];

	fp_commands = fopen ( COMMANDS_FILE, "r" );

	bzero ( s_line, BUF_LEN );

	while ( fgets ( s_line, sizeof s_line, fp_commands) != NULL ) {// Read line
		/*
		   c = strchr ( s_line, '\n' );
		//Remove trailing \n
		if (c) {
		 *c = 0;
		 }
		 */
		/* Tokenizing */
		bzero (s_buffer, BUF_LEN);
		bzero (command, BUF_LEN);
		bzero (message, BUF_LEN);
		token = strtok (s_line, " ");
		strncpy (command, token, BUF_LEN);

		if ( strcmp(command, "WRITE") == 0 ) {
			token = strtok (NULL, " \"");
		} else {
			token = strtok (NULL, "\n");
		}
		value = atoi(token);

		if ( strcmp ( command, "WAIT" ) == 0 ) {
			if ( value == -1 ) {
				fprintf ( stdout, "Press ENTER to continue\n");
				getchar();
			} else {
				usleep ( value * _MSEC );
			}
		} else {
			if ( strcmp(command, "WRITE") == 0 ) {
				token = strtok (NULL, "\"\n");
				strncpy (message, token, BUF_LEN);
				sprintf (s_buffer, "%s %s", command, message);
			} else {
				sprintf (s_buffer, "%s", command);
			}
			write ( sock_tcp[value], s_buffer, BUF_LEN );
		}

		/* Tokenizing */

		bzero ( s_line, BUF_LEN );
	}
	return;
}
