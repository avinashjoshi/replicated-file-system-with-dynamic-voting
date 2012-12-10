#include "server_header.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/*
 * This function resolves hostname 
 * from IP address
 *
 * Returns the resolved "hostname"
 */
char *
resolve_hostname ( char *s_ip ) {
	struct hostent *he;
	struct in_addr ipv4addr;

	inet_pton(AF_INET, s_ip, &ipv4addr);
	he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

	return (he->h_name);
}

/*
 * This is the main function forked by udp_recv_init
 * that waits on recvfrom() and inserts the received
 * data to request queue
 */
void
*handle_udp ( void *port ) {
	int ret_recv;
	char s_buffer[BUF_LEN];
	char *ptr_buf = s_buffer;
	struct sockaddr_in addr_client;
	int n_buffer = sizeof(s_buffer);
	socklen_t n_addr_client = sizeof(addr_client);

	while (1) {
		bzero ( ptr_buf, BUF_LEN );
		ret_recv = recvfrom(sock_udp_recv, ptr_buf, n_buffer, 0, (struct sockaddr *)&addr_client, &n_addr_client);
		if ( strcmp(ptr_buf, "NODE-DOWN") == 0 ) {
			break;
		}
		if(ret_recv < 0) {
			close(sock_udp_recv);
			diep("[UDP-SERVER] recvfrom() error");
		}

		log_info("[UDP-SERVER] RECEIVED (%s) %s:%d - \"%s\"", resolve_hostname (inet_ntoa(addr_client.sin_addr)),
				inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port), ptr_buf);
	}
	close(sock_udp_recv);
	log_info ("[UDP-SERVER] Shutting down");

	return NULL;
}

/*
 * Initialize UDP Receive - Create socket
 */
void
udp_recv_init ( int i_port_value ) {
	int err;
	long int u_port = i_port_value;
	int ret_bind;
	struct sockaddr_in addr_server;
	socklen_t n_addr_server = sizeof(addr_server);

	if((sock_udp_recv = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		diep("[UDP-SERVER] socket() error");
	} else {
		log_info("[UDP-SERVER] socket() is OK");
	}

	memset(&addr_server, 0x00, n_addr_server);
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(u_port);
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	if((ret_bind = bind(sock_udp_recv, (struct sockaddr *)&addr_server, n_addr_server)) < 0) {
		close(sock_udp_recv);
		diep("[UDP-SERVER] bind() error");
		/* If something wrong with socket(), just exit lol */
	} else {
		log_info("[UDP-SERVER] bind() is OK");
	}

	err = pthread_create ( &udp_thread, NULL, handle_udp, (void *) u_port );

	ASSERT ( (err == 0), "[UDP-SERVER] Unable to create thread");

	log_info ("[UDP-SERVER] Thread created");
}

/*
 * Implements the actual sendt()
 *
 * Takes the IP/hostname, port and message to be sent
 * and writes it to the network on the UDP port
 */
void
udp_send ( char *s_to_whom, int i_the_port, char *s_the_message ) {
	int ret_send;
	struct sockaddr_in addr_server;
	char s_buffer[BUF_LEN];
	char s_serv_name[HOST_SIZE];
	char *ptr_buf = s_buffer;
	int len_buffer = sizeof(s_buffer);
	struct hostent *ptr_host;
	int u_port;

	memset(s_buffer, 0x00, sizeof(s_buffer));
	u_port = i_the_port;
	strncpy ( s_buffer, s_the_message, BUF_LEN );
	strncpy ( s_serv_name, s_to_whom, HOST_SIZE );

	memset(&addr_server, 0x00, sizeof(struct sockaddr_in));
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(u_port);

	if((addr_server.sin_addr.s_addr = inet_addr(s_serv_name)) == (unsigned long)INADDR_NONE) {
		ptr_host = gethostbyname(s_serv_name);
		if(ptr_host == (struct hostent *)NULL) {
			log_err("%s HOST NOT FOUND", s_serv_name);
			/* h_errno is usually defined */
			/* in netdb.h */
			//printf("h_errno = %d\n", h_errno);
		}
		memcpy(&addr_server.sin_addr, ptr_host->h_addr, sizeof(addr_server.sin_addr));
	}

	ret_send = sendto (sock_udp_send, ptr_buf, len_buffer, 0, (struct sockaddr *)&addr_server, sizeof(addr_server));
	if(ret_send < 0) {
		log_err("[UDP-SENDER] sendto() error");
	}
}

/*
 * Initialize UPD Sender - Create socket for a sender
 */
void
udp_send_init ( void ) {
	if( (sock_udp_send = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		diep("[UDP-SENDER] socket() error");
	} else {
		log_info("[UDP-SENDER] Created Socket for UDP Sender!");
	}
}

/*
 * Sends a "PING" packet to all UDP servers from serv_list
 */
void
ping_servers ( void ) {
	int i_serv, u_port;
	u_port = PORT;

	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		udp_send (serv_list[i_serv].name, PORT, "PING");
	}
}
