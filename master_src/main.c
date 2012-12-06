#include "master_header.h"

#define SERVER "net01.utdallas.edu"

int
main ( int argc, char *argv[] ) {

	prog_name = get_program_name ( argv[0] );
	gethostname (s_hostname, sizeof s_hostname);
	strcpy ( s_hostname_short, s_hostname);
	strtok(s_hostname_short, ".");
	sprintf (s_log_filename, "logs/%s", s_hostname_short);

	fp_log = fopen ( s_log_filename, "w" );

	ASSERT ( fp_log, " - folder 'logs' does not exist");

	fclose(fp_log);

	parse_config();
	tcp_send_init ( );

	pthread_join ( tcp_thread, NULL );

	return EXIT_SUCCESS;
}
