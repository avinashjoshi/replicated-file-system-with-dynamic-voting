#include "server_header.h"
#include <pthread.h>

void
global_init ( void ) {
	sock_tcp = -1;
	tcp_q = NULL;
	udp_q = NULL;
	my_status = DOWN;
	vn = 0;
	ru = TOTAL_SERVERS;
	strcpy(ds, "0");
	recv_votes = 0;
	is_voting = FALSE;
	reset_timers();
	reset_servers();
}

int
main ( int argc, char *argv[] ) {

	global_init ();

	prog_name = get_program_name ( argv[0] );
	gethostname (s_hostname, sizeof s_hostname);
	strcpy ( s_hostname_short, s_hostname);
	strtok(s_hostname_short, ".");
	sprintf (s_log_filename, "logs/%s", s_hostname_short);
	sprintf (s_out_filename, "file_servers/%s", s_hostname_short);

	my_id = get_serv_index(s_hostname);

	fp_log = fopen ( s_log_filename, "w" );
	ASSERT ( fp_log, " - folder 'logs' does not exist");
	fclose(fp_log);

	fp_out = fopen ( s_out_filename, "w" );
	ASSERT ( fp_out, " - folder 'file_servers' does not exist");
	fclose(fp_out);

	/*
	   queue *ret_q;

	   insert_queue ( &tcp_q, "net01", "ONE");
	   insert_queue ( &tcp_q, "net01", "TWO");
	   insert_queue ( &tcp_q, "net03", "ONE");
	   ret_q = remove_queue ( &tcp_q );
	   printf ("%s\n", ret_q->data);
	   print_queue ( tcp_q );
	   ret_q = remove_queue ( &tcp_q );
	   print_queue ( tcp_q );
	   printf ("%s\n", ret_q->data);
	   printf ("%d\n", is_queue_empty ( tcp_q ));

	   exit (EXIT_SUCCESS);
	   */

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

	/* Making sure that there is a TCP connection with the master */
	while ( sock_tcp != -1 );

	//ping_servers();

	pthread_create ( &tcp_queue_thread, NULL, handle_tcp_queue, NULL);
	pthread_create ( &udp_queue_thread, NULL, handle_udp_queue, NULL);
	pthread_create ( &udp_timer, NULL, handle_timer, NULL);

	pthread_join ( udp_thread, NULL );
	pthread_join ( tcp_thread, NULL );
	pthread_join ( tcp_queue_thread, NULL );
	pthread_join ( udp_queue_thread, NULL );

	return EXIT_SUCCESS;
}
