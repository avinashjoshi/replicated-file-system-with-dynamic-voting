#include "server_header.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void
tcp_recv_init ( int t_port ) {
	int sd, rc, length = sizeof(int);
	struct sockaddr_in addr_server;

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

		/* Server does not echo back!
		 * - only receives and put in queue
		 *
		if (send(sock_tcp, buffer, rc, 0) != rc)
			diep("[TCP-SERVER] send() failed");
		bzero ( buffer, BUF_LEN );
		*/

		/* See if there is more data to receive */
		if ((rc = recv(sock_tcp, buffer, BUF_LEN, 0)) < 0)
			diep("[TCP-SERVER] recv() failed");
	}

	log_info ("[TCP-SERVER] Closing TCP Connection");

	return NULL;
}
