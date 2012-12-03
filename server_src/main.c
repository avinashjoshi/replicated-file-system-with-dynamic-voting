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

	udp_init ( u_port );

	sleep(3);

#ifdef _TEST_
	test_udp_send ( u_port );
#endif

	pthread_join ( udp_thread, NULL );

	return EXIT_SUCCESS;
}
