#include "server_header.h"

#include <pthread.h>

/*
 * Initialize the TCP receiver
 *   - create socket
 *   - fork thread
 */
void
tcp_recv_init ( int t_port ) {
	int sd, rc, length = sizeof(int);
	struct sockaddr_in addr_server;

	/* Create socket */
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("[TCP-SERVER] socket() error");
		/* Just exit */
		exit (-1);
	} else {
		log_info ("[TCP-SERVER] socket() is OK");
	}

	memset(&addr_server, 0x00, sizeof(struct sockaddr_in));
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(t_port);
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

	if((rc = bind(sd, (struct sockaddr *)&addr_server, sizeof(addr_server))) < 0) {
		perror("[TCP-SERVER] bind() error");
		/* Close the socket descriptor */
		close(sd);
		/* and just exit */
		exit(-1);
	} else {
		log_info("[TCP-SERVER] bind() is OK");
	}

	if((rc = listen(sd, 10)) < 0) {
		perror("[TCP-SERVER] listen() error");
		close(sd);
		exit (-1);
	} else {
		log_info ("[TCP-SERVER] listen() is OK");
	}

	int err;

	err = pthread_create ( &tcp_thread, NULL, handle_tcp, sd);
	ASSERT ( (err == 0), "[TCP-SERVER] - Unable to create TCP thread");

}

/*
 * Actual function that handles all
 * incoming TCP connections
 */
void
*handle_tcp ( void *sd ) {
	char buffer[BUF_LEN];
	int rc;
	struct sockaddr_in addr_master;
	int sin_size = sizeof(struct sockaddr_in);
	if((sock_tcp = accept(sd, (struct sockaddr *)&addr_master, &sin_size)) < 0) {
		perror("[TCP-SERVER] accept() error");
		close(sd);
		exit (-1);
	} else {
		log_info("[TCP-SERVER] accept() is OK");
	}

	/*client IP*/
	log_info("[TCP-SERVER] Master's IP: %s", inet_ntoa(addr_master.sin_addr));

	bzero ( buffer, BUF_LEN );

	/* Receive message from client */
	if ((rc = recv(sock_tcp, buffer, BUF_LEN, 0)) < 0)
		diep("[TCP-SERVER] recv() failed");

	/* Send received string and receive again until end of transmission */
	while (rc > 0) {
		log_info("[TCP-SERVER] RECEIVED %s (%s) - %s", inet_ntoa(addr_master.sin_addr), resolve_hostname(inet_ntoa(addr_master.sin_addr)), buffer);

		pthread_mutex_lock ( &lock_tcp_q );
		insert_queue ( &tcp_q, inet_ntoa(addr_master.sin_addr), buffer );
		pthread_mutex_lock ( &lock_tcp_q );

		if ( strcmp (buffer, "HALT") == 0) {
			break;
		}

		/* See if there is more data to receive */
		if ((rc = recv(sock_tcp, buffer, BUF_LEN, 0)) < 0)
			diep("[TCP-SERVER] recv() failed");
	}

	if ( rc == 0 ) {
		log_info("[TCP-SERVER] Master closed connection");
	}

	log_info ("[TCP-SERVER] Closing TCP Connection");

	return NULL;
}

void
*handle_tcp_queue ( void *text ) {
	queue *ret_q;
	char s_buffer[BUF_LEN];
	int i_can_send;
	while (1) {
		bzero ( s_buffer, BUF_LEN);
		i_can_send = FALSE;
		pthread_mutex_lock ( &lock_tcp_q );
		if ( (ret_q = remove_queue ( &tcp_q ) ) == NULL ) {
			pthread_mutex_unlock ( &lock_tcp_q );
			usleep ( 300 );
			continue;
		}
		pthread_mutex_unlock ( &lock_tcp_q );
		printf ("REMOVED QUEUE: %s: %s\n", ret_q->host, ret_q->data);
		if (strcmp(ret_q->data, "PING") == 0 ) {
			sprintf ( s_buffer, "REPLY" );
			i_can_send = TRUE;
		} else if ( strcmp(ret_q->data, "HALT") == 0 ) {
			udp_send ( "localhost", PORT, "NODE-DOWN");
			break;
		} else if ( strcmp(ret_q->data, "NODE-DOWN") == 0 ) {
			udp_send ( "localhost", PORT, "NODE-DOWN");
			break;
		} else {
			sprintf ( s_buffer, "UNKNOWN REPLY" );
			i_can_send = TRUE;
		}

		if ( i_can_send ) {
			send(sock_tcp, s_buffer, BUF_LEN, 0);
		}
	} // end while (1)
	log_info ("[TCP-SERVER] Closing the handle_receive()");
	return NULL;
}
