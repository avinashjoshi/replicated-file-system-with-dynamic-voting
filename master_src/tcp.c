#include "master_header.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void
tcp_send_init ( int t_port ) {
	int rc, length = sizeof(int);
	struct sockaddr_in serveraddr;
	char buffer[BUF_LEN];
	char server[HOST_SIZE];
	struct hostent *hp_host;
	char data[100] = "HELLO";
	int i_serv;

	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++) {

		if((sock_tcp[i_serv] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("[TCP-SENDER] socket() error");
			exit(-1);
		} else {
			log_info("[TCP-SENDER] socket() OK");
		}

		strcpy ( server, serv_list[i_serv].name);


		memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(t_port);

		if((serveraddr.sin_addr.s_addr = inet_addr(server)) == (unsigned long)INADDR_NONE)
		{

			/* When passing the host name of the server as a */
			/* parameter to this program, use the gethostbyname() */
			/* function to retrieve the address of the host server. */
			/***************************************************/
			/* get host address */
			hp_host = gethostbyname(server);
			if(hp_host == (struct hostent *)NULL) {
				log_err ("HOST NOT FOUND");
				/* h_errno is usually defined */
				/* in netdb.h */
				//printf("h_errno = %d",h_errno);
				close(sock_tcp[i_serv]);
				exit(-1);
			}
			memcpy(&serveraddr.sin_addr, hp_host->h_addr, sizeof(serveraddr.sin_addr));
		}

		/* After the socket descriptor is received, the */
		/* connect() function is used to establish a */
		/* connection to the server. */
		/***********************************************/
		/* connect() to server. */
		if((rc = connect(sock_tcp[i_serv], (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0) {
			perror("[TCP-SENDER] connect() error");
			close(sock_tcp[i_serv]);
			exit(-1);
		} else {
			log_info("[TCP-SENDER] Connection established...");
		}


		log_info("[TCP-SENDER] Sending \"%s\" %s:%d", data, serv_list[i_serv].name, t_port);
		rc = write(sock_tcp[i_serv], data, sizeof(data));

		if(rc < 0) {
			perror("[TCP-SENDER] write() error");
			close(sock_tcp[i_serv]);
			exit(-1);
		} else {
			log_info("[TCP-SENDER] write() is OK");
		}

		rc = pthread_create ( &tcp_thread, NULL, handle_tcp, i_serv);
		ASSERT ( (rc == 0), "[TCP-SERVER] - Unable to create TCP thread");
	}
}

void
*handle_tcp ( void *value ) {
	int i_serv = (int) value;
	char buffer[BUF_LEN];
	int rc;

	bzero ( buffer, BUF_LEN );

	/* Receive message from client */
	if ((rc = recv(sock_tcp[i_serv], buffer, BUF_LEN, 0)) < 0)
		diep("[TCP-SERVER] recv() failed");

	/* Send received string and receive again until end of transmission */
	while (rc > 0) {
		log_info("[TCP-SERVER] RECEIVED %s - %s", serv_list[i_serv].name, buffer);
		/* Echo message back to client */
		if (send(sock_tcp[i_serv], buffer, rc, 0) != rc)
			diep("[TCP-SERVER] send() failed");
		bzero ( buffer, BUF_LEN );

		/* See if there is more data to receive */
		if ((rc = recv(sock_tcp[i_serv], buffer, BUF_LEN, 0)) < 0)
			diep("[TCP-SERVER] recv() failed");
	}

	log_info ("[TCP-SERVER] Closing TCP Connection");

	return NULL;
}

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

