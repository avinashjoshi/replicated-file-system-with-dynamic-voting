#include "master_header.h"

#include <pthread.h>

void
tcp_send_init ( void ) {
	int i_serv;
	int rc;

	/*
	 * Create thread for every s_name
	 */
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++) {
		rc = pthread_create ( &tcp_thread, NULL, handle_tcp, i_serv);
		ASSERT ( (rc == 0), "[TCP-SERVER] - Unable to create TCP thread");
	}
}

/*
 * Thread that handles connection with each server
 */
void
*handle_tcp ( void *value ) {
	int t_port = PORT;
	int i_serv = (int) value;
	int rc, length = sizeof(int);
	struct sockaddr_in addr_serv;
	char buffer[BUF_LEN];
	char s_name[HOST_SIZE];
	char s_cname[HOST_SIZE];
	struct hostent *hp_host;
	char data[100] = "HELLO";

	strcpy ( s_name, serv_list[i_serv].name);
	strcpy ( s_cname, serv_list[i_serv].c_name);

	if((sock_tcp[i_serv] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_err("[TCP-SENDER] %s: socket() error", s_cname);
		exit(-1);
	} else {
		log_info("[TCP-SENDER] %s: socket() OK", s_cname);
	}

	memset(&addr_serv, 0x00, sizeof(struct sockaddr_in));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(t_port);

	if((addr_serv.sin_addr.s_addr = inet_addr(s_name)) == (unsigned long)INADDR_NONE)
	{

		/* When passing the host name of the s_name as a */
		/* parameter to this program, use the gethostbyname() */
		/* function to retrieve the address of the host s_name. */
		/***************************************************/
		/* get host address */
		hp_host = gethostbyname(s_name);
		if(hp_host == (struct hostent *)NULL) {
			log_err ("[TCP-SENDER] %s: host not found", s_cname);
			/* h_errno is usually defined */
			/* in netdb.h */
			//printf("h_errno = %d",h_errno);
			close(sock_tcp[i_serv]);
			exit(-1);
		}
		memcpy(&addr_serv.sin_addr, hp_host->h_addr, sizeof(addr_serv.sin_addr));
	}

	/* After the socket descriptor is received, the */
	/* connect() function is used to establish a */
	/* connection to the s_name. */
	/***********************************************/
	/* connect() to s_name. */
	if((rc = connect(sock_tcp[i_serv], (struct sockaddr *)&addr_serv, sizeof(addr_serv))) < 0) {
		log_err("[TCP-SENDER] %s: connect() error", s_cname);
		close(sock_tcp[i_serv]);
		exit(-1);
	} else {
		log_info("[TCP-SENDER] %s: connect() Connection established", s_cname);
	}


	log_info("[TCP-SENDER] Sending \"%s\" %s:%d", data, s_cname, t_port);
	rc = write(sock_tcp[i_serv], data, sizeof(data));

	if(rc < 0) {
		log_err("[TCP-SENDER] %s: write() error", s_cname);
		close(sock_tcp[i_serv]);
		exit(-1);
	} else {
		log_info("[TCP-SENDER] %s: write() is OK", s_cname);
	}

	bzero ( buffer, BUF_LEN );

	/* Receive message from client */
	if ((rc = recv(sock_tcp[i_serv], buffer, BUF_LEN, 0)) < 0)
		diep("[TCP-SERVER] recv() failed");

	/* Send received string and receive again until end of transmission */
	while (rc > 0) {
		log_info("[TCP-SERVER] RECEIVED %s - %s", s_cname, buffer);
		/* Echo message back to client */
		if (send(sock_tcp[i_serv], buffer, rc, 0) != rc)
			diep("[TCP-SERVER] send() failed");
		bzero ( buffer, BUF_LEN );
		break;

		/* See if there is more data to receive */
		if ((rc = recv(sock_tcp[i_serv], buffer, BUF_LEN, 0)) < 0)
			diep("[TCP-SERVER] recv() failed");
	}

	log_info ("[TCP-SERVER] Closing TCP Connection");

	return NULL;
}
