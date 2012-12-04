#include "server_header.h"
#include <pthread.h>

int
main ( int argc, char *argv[] ) {
	debug ("I am a SERVER");

	int u_port = PORT;

	// Parse the config file
	parse_config();
	
#ifdef _TEST_
	print_con_list();
#endif

	udp_recv_init ( u_port );
	udp_send_init();

	sleep(3);

	ping_servers();

	pthread_join ( udp_thread, NULL );

	return EXIT_SUCCESS;
}
