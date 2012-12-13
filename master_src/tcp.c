#include "master_header.h"

#include <pthread.h>

void
tcp_send_init ( void ) {
	long int i_serv;
	int rc;

	/*
	 * Create thread for every s_name
	 */
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++) {
		rc = pthread_create ( &tcp_thread[i_serv], NULL, handle_tcp, (void *) i_serv);
		ASSERT ( (rc == 0), "[TCP-SERVER] - Unable to create TCP thread");
	}
}

/*
 * Thread that handles connection with each server
 */
void
*handle_tcp ( void *value ) {
	int t_port = PORT;
	int i_serv = (long int) value;
	int rc;
	struct sockaddr_in addr_serv;
	char buffer[BUF_LEN];
	char s_name[HOST_SIZE];
	char s_cname[HOST_SIZE];
	struct hostent *hp_host;
	char data[100] = "PING";

	strcpy ( s_name, serv_list[i_serv].name);
	strcpy ( s_cname, serv_list[i_serv].c_name);

	pthread_mutex_lock ( &lock_tcp_sock );
	if((sock_tcp[i_serv] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_err("[TCP-SENDER] %s: socket() error", s_cname);
		pthread_mutex_unlock ( &lock_tcp_sock );
		exit(-1);
	} else {
		log_info("[TCP-SENDER] %s: socket() OK", s_cname);
	}

	memset(&addr_serv, 0x00, sizeof(struct sockaddr_in));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(t_port);

	if((addr_serv.sin_addr.s_addr = inet_addr(s_name)) == (unsigned long)INADDR_NONE)
	{

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
		//log_info("[TCP-SENDER] %s: write() is OK", s_cname);
	}

	pthread_mutex_unlock ( &lock_tcp_sock );
	bzero ( buffer, BUF_LEN );

	/* Send received string and receive again until end of transmission */
	while ( (rc = recv(sock_tcp[i_serv], buffer, BUF_LEN, 0)) > 0) {

		write_file("[RECEIVED] %s: %s", s_cname, buffer);

		if ( strcmp (buffer, "PONG") == 0) {
			pthread_mutex_lock(&lock_tcp_sock);
			tcp_replies ++;
			pthread_mutex_unlock(&lock_tcp_sock);
		}

		if ( strcmp (buffer, "HALTED" ) == 0 ) {
			log_info ("[TCP-SEVER] Halting confirmed by %s", s_cname);
		}

		bzero ( buffer, BUF_LEN );
	}

	/* Receive message from client */
	if ( rc < 0 ) {
		log_err("[TCP-SERVER] recv() failed");
	}

	if ( rc == 0 ) {
		log_info ( "[TCP-SERVER] Connection closed by %s", s_cname);
	}

	log_info ("[TCP-SERVER] Closing TCP Connection");

	return NULL;
}
