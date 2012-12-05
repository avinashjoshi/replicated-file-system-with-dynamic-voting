#include "server_header.h"
#include <pthread.h>

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

	int u_port = PORT;

	// Parse the config file
	parse_config();
	
#ifdef _TEST_
	print_con_list();
#endif

	udp_recv_init ( u_port );
	udp_send_init();
	tcp_recv_init(PORT);

	sleep(3);

	//ping_servers();

	pthread_join ( udp_thread, NULL );
	pthread_join ( tcp_thread, NULL );

	return EXIT_SUCCESS;
}
