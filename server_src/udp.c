#include "server_header.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void
*handle_udp ( void *port ) {
	int sd, rc;
	int u_port = (long int) port;
	struct sockaddr_in addr_server, addr_client;
	socklen_t n_addr_client = sizeof(addr_client);
	socklen_t n_addr_server = sizeof(addr_server);
	char s_buffer[BUF_LEN];
	char *ptr_buf = s_buffer;
	int n_buffer = sizeof(s_buffer);

	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		diep("UDP server - socket() error");
	} else {
		log_info("UDP server - socket() is OK");
	}

	memset(&addr_server, 0x00, n_addr_server);
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(u_port);
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	if((rc = bind(sd, (struct sockaddr *)&addr_server, n_addr_server)) < 0) {
		close(sd);
		diep("UDP server - bind() error");
		/* If something wrong with socket(), just exit lol */
	} else {
		log_info("UDP server - bind() is OK");
	}

	while (1) {
		printf ("Waiting to receive...\n");
		rc = recvfrom(sd, ptr_buf, n_buffer, 0, (struct sockaddr *)&addr_client, &n_addr_client);
		if(rc < 0) {
			close(sd);
			diep("UDP Server - recvfrom() error");
		} else {
			log_info("UDP Server - recvfrom() is OK...\n");
		}

		printf("[RECEIVED] %s:%d - \"%s\"\n",
				inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port), ptr_buf);
	}
	log_err ("Exitting :(");

	return NULL;
}

void
udp_init ( int u_port ) {
	int err;

	err = pthread_create ( &udp_thread, NULL, handle_udp, u_port );

	ASSERT ( (err == 0), " - Unable to create thread");

	log_info ("Thread created");
}

#ifdef _TEST_
void
test_udp_send ( int u_port ) {
	int sd, rc;
	struct sockaddr_in addr_server;
	char s_buffer[BUF_LEN];
	char *ptr_buf = s_buffer;
	int len_buffer = sizeof(s_buffer);
	int i_serv;
	struct hostent *ptr_host;

	memset(s_buffer, 0x00, sizeof(s_buffer));

	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		diep("UDP Client - socket() error");
	} else {
		log_info("UDP Client - socket() is OK!");
	}

	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {

		sprintf ( s_buffer, "HELLO");

		memset(&addr_server, 0x00, sizeof(struct sockaddr_in));
		addr_server.sin_family = AF_INET;
		addr_server.sin_port = htons(u_port);

		if((addr_server.sin_addr.s_addr = inet_addr(serv_list[i_serv].name)) == (unsigned long)INADDR_NONE) {
			ptr_host = gethostbyname(serv_list[i_serv].name);
			if(ptr_host == (struct hostent *)NULL) {
				log_err("HOST NOT FOUND --> ");
				/* h_errno is usually defined */
				/* in netdb.h */
				printf("h_errno = %d\n", h_errno);
				exit(-1);
			} else {
				log_info("UDP Client - gethostname() of the server is OK...");
			}
			memcpy(&addr_server.sin_addr, ptr_host->h_addr, sizeof(addr_server.sin_addr));
		}

		log_info ("Sending to => %s", serv_list[i_serv].name);

		rc = sendto(sd, ptr_buf, len_buffer, 0, (struct sockaddr *)&addr_server, sizeof(addr_server));
		log_info ("SENT");
		if(rc < 0) {
			close(sd);
			diep("UDP Client - sendto() error");
		}
	}
}
#endif
