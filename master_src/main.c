#include "master_header.h"

void
global_init ( void ) {
	tcp_replies = 0;
}

int
main ( int argc, char *argv[] ) {

	FILE *fp_commands;
	prog_name = get_program_name ( argv[0] );
	gethostname (s_hostname, sizeof s_hostname);
	strcpy ( s_hostname_short, s_hostname);
	strtok(s_hostname_short, ".");
	sprintf (s_log_filename, "logs/%s", s_hostname_short);

	fp_log = fopen ( s_log_filename, "w" );
	ASSERT ( fp_log, " - folder 'logs' does not exist");
	fclose(fp_log);

	fp_commands = fopen ( COMMANDS_FILE, "r" );
	ASSERT ( fp_commands, " - %s", COMMANDS_FILE );
	fclose (fp_commands);

	parse_config();
	tcp_send_init();

	while ( 1 ) {
		pthread_mutex_lock(&lock_tcp_sock);
		if (tcp_replies == TOTAL_SERVERS ) {
			pthread_mutex_unlock(&lock_tcp_sock);
			break;
		}
		pthread_mutex_unlock(&lock_tcp_sock);
		usleep(300);
	}
	log_info ("Received connection from all SERVERS");

	/* Handling the commands */
	handle_servers();

	/*
	   sleep(5);
	   log_info ("[TCP-SERVER] Closing all TCP sockets");
	   close_tcp_socks();
	   */

	pthread_join ( tcp_thread, NULL );

	return EXIT_SUCCESS;
}
