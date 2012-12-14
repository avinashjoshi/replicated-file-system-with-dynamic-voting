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

void
*handle_timer ( void *text ) {
	int i_serv;
	while ( 1 ) {
		usleep ( TIMEOUT * _MSEC );
		for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
			if ( T[i_serv] >= 20 ) {
				pthread_mutex_lock ( &lock_p );
				P[i_serv] = DOWN;
				pthread_mutex_unlock ( &lock_p );
				ping (i_serv);
			} else {
			}
			T[i_serv] += 10;
		}
		if ( my_status == HALTED )
			break;
	}
	return NULL;
}

void
reset_votes ( void ) {
	int i_voter;
	for ( i_voter = 0; i_voter < TOTAL_SERVERS; i_voter++ ) {
		votes[i_voter].status = DOWN;
	}
}

int
get_serv_letter ( int integer ) {
	return ( 65 + integer );
}

/*
 * Returns true if all of
 * who belongs to whom
 */
int
belongs_to ( char *who, char *whom ) {
	int i_who, i_whom;
	int flag = FALSE;
	for ( i_who = 0; i_who < strlen(who); i_who++ ) {
		flag = FALSE;
		for ( i_whom = 0; i_whom < strlen(whom); i_whom++ ) {
			if ( who[i_whom] == whom[i_whom] ) {
				flag = TRUE;
			}
		}
		if ( flag == FALSE )
			break;
	}
	return flag;
}

int
is_distinguished ( ) {
	M = 0;
	char s_letter[] = "0";
	int N;
	int i_serv;
	strcpy ( I, "");
	/* M = MAX {VN_j :S_j belongsto P} */
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		if ( votes[i_serv].status == DOWN )
			continue;
		if (votes[i_serv].vn > M) {
			M = votes[i_serv].vn;
		}
	} // Done getting Max VN
	/* Getting I */
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		if ( votes[i_serv].status == DOWN )
			continue;
		if (votes[i_serv].vn == M) {
			sprintf ( s_letter, "%c", get_serv_letter(i_serv));
			strcat ( I, s_letter );
			N = votes[i_serv].ru;
		}
	}

	if ( strlen (I) > N/2 )
		return TRUE;
	else if ( strlen (I) == N/2 )
		if ( belongs_to ( ds, I ) )
			return TRUE;
		else
			return FALSE;
	else
		return FALSE;

}

int
is_latest_copy ( void ) {
	int i_serv;
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		if ( votes[i_serv].status == DOWN )
			continue;
		if ( vn < votes[i_serv].vn )
			return FALSE;
	}
	return TRUE;
}

int
total_voted_servers ( void ) {
	int i_serv;
	int total = 0;
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		if ( votes[i_serv].status == DOWN )
			continue;
		total ++;
	}
	return total;
}

int
is_even ( int number ) {
	if ( (number % 2) == 0 ) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void
fault_tolerance ( void ) {
	reset_votes();
	send_vote_request();
	usleep ( NETWORK_DELAY * _MSEC );
	if ( is_distinguished() ) {
		if ( vn <  M ) {
			/* obtain latest copy */
		}
	}
}

int
vote_request_rw ( int read_write ) {
	int ret_val = FALSE;
	while ( 1 ) {
		pthread_mutex_lock ( &lock_voting );
		if ( is_voting == TRUE ) {
			pthread_mutex_unlock ( &lock_voting );
			usleep ( NETWORK_DELAY * _MSEC );
			continue;
		} else {
			is_voting = TRUE;
			break;
		}
	}
	reset_votes();
	send_vote_request();
	//while ( recv_votes < total_voters-1 );
	usleep ( NETWORK_DELAY * _MSEC );

	int i;
	for ( i = 0; i < TOTAL_SERVERS; i++ ) {
		if ( votes[i].status == DOWN )
			continue;
		log_info ( "%d:<%d,%d,%s>", i, votes[i].vn, votes[i].ru, votes[i].ds);
	}

	if ( is_distinguished() ) {
		if ( is_latest_copy() == FALSE ) {
			/* get latest copy */
		}
		int i_serv;
		char s_temp[] = "0";
		ret_val = TRUE;
		if ( read_write == WRITE ) {
			vn = M + 1;
			ru = total_voted_servers(); 
			strcpy ( ds, "" );
			/* P_i If |P_i|=3 */
			if ( ru == 3 ) {
				for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
					if ( votes[i_serv].status == DOWN  )
						continue;
					sprintf ( s_temp, "%c", get_serv_letter(i_serv));
					strcat ( ds, s_temp);
				}
			} else if ( (ru > 3) && is_even(ru) ) {
				for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
					if ( votes[i_serv].status == DOWN  )
						continue;
					sprintf ( s_temp, "%c", get_serv_letter(i_serv));
					strcat ( ds, s_temp);
					break;
				}
			}
		}
	}

	is_voting = FALSE;
	pthread_mutex_unlock ( &lock_voting );
	return ret_val;
}

void
*handle_udp_queue ( void *text) {
	queue *ret_q;
	char s_buffer[BUF_LEN];
	char s_buffer_temp[BUF_LEN];
	char *token, *tok_saveptr;
	int i_can_send;
	int host_index;
	while (1) {
		bzero ( s_buffer, BUF_LEN);
		i_can_send = FALSE;
		pthread_mutex_lock ( &lock_udp_q );
		if ( (ret_q = remove_queue ( &udp_q ) ) == NULL ) {
			pthread_mutex_unlock ( &lock_udp_q );
			usleep ( 300 );
			continue;
		}
		pthread_mutex_unlock ( &lock_udp_q );
		//printf ("REMOVED QUEUE: %s: %s\n", ret_q->host, ret_q->data);
		host_index = get_serv_index ( ret_q->host );
		if (strcmp(ret_q->data, "PING") == 0 ) {
			sprintf ( s_buffer, "PONG" );
			i_can_send = TRUE;
		} else if (strcmp(ret_q->data, "HALT") == 0 ) {
			break;
		} else if (strcmp(ret_q->data, "VOTE-REQUEST") == 0 ) {
			sprintf ( s_buffer, "VOTE-REPLY<%d,%d,%s>", vn, ru, ds);
			i_can_send = TRUE;
		} else {
			strcpy ( s_buffer_temp, ret_q->data );
			token = strtok_r ( s_buffer_temp, "<", &tok_saveptr);
			if ( token == NULL ) {
				sprintf ( s_buffer, "UNKNOWN-COMMAND" );
				i_can_send = TRUE;
			}
			if ( strcmp(token, "COMMIT") == 0 ) {
				token = strtok_r ( NULL, ">", &tok_saveptr);
				if ( token != NULL ) {
					write_file ( "<%s>", token);
				}
			}
			if ( strcmp(token, "VOTE-REPLY") == 0 ) {
				token = strtok_r ( NULL, ",", &tok_saveptr);
				votes[host_index].vn = atoi ( token );
				token = strtok_r ( NULL, ",", &tok_saveptr);
				votes[host_index].ru = atoi ( token );
				token = strtok_r ( NULL, ">", &tok_saveptr);
				strcpy ( votes[host_index].ds, token );
				votes[host_index].status = UP;
				recv_votes ++;
			} else {
				sprintf ( s_buffer, "UNKNOWN-COMMAND" );
				i_can_send = TRUE;
			}
		}

		if ( i_can_send ) {
			udp_send ( ret_q->host, PORT, s_buffer);
		}
	} // end while (1)
	log_info ("[UDP-SERVER] Closing the handle_udp_queue()");

	return NULL;
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
	int recv_host_index;
	char s_recv_hostname [HOST_SIZE];
	socklen_t n_addr_client = sizeof(addr_client);

	pthread_mutex_lock ( &lock_status );
	my_status = UP;
	pthread_mutex_unlock ( &lock_status );
	log_info ("[UDP-SERVER] Started");

	while (1) {
		bzero ( ptr_buf, BUF_LEN );
		bzero ( s_recv_hostname, HOST_SIZE );
		ret_recv = recvfrom(sock_udp_recv, ptr_buf, n_buffer, 0, (struct sockaddr *)&addr_client, &n_addr_client);

		if(ret_recv < 0) {
			close(sock_udp_recv);
			diep("[UDP-SERVER] recvfrom() error");
		}

		strcpy ( s_recv_hostname, resolve_hostname (inet_ntoa(addr_client.sin_addr)) );
		recv_host_index = get_serv_index(s_recv_hostname);

		if ( strcmp(ptr_buf, "NODE-DOWN") == 0 ) {
			log_info ("[UDP-SERVER] Master requested NODE-DOWN");
			pthread_mutex_lock ( &lock_status );
			my_status = DOWN;
			pthread_mutex_unlock ( &lock_status );
			break;
		}

		if ( ! (strcmp ( ptr_buf, "PING") == 0) ) {
			pthread_mutex_lock ( &lock_status );
			P[recv_host_index] = UP;
			T[recv_host_index] = 0;
			pthread_mutex_unlock ( &lock_status );
			if ( ! (strcmp ( ptr_buf, "PONG") == 0) ) {
				log_info("[UDP-SERVER] RECEIVED %d:%s {%s}", recv_host_index, s_recv_hostname, ptr_buf);
			} else {
				continue;
			}
		} else {
			udp_send ( s_recv_hostname, PORT, "PONG");
			continue;
		}

		pthread_mutex_lock ( &lock_udp_q );
		insert_queue ( &udp_q, s_recv_hostname, ptr_buf );
		pthread_mutex_unlock ( &lock_udp_q );

	}

	close(sock_udp_recv);
	log_info ("[UDP-SERVER] Shutdown");

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
	int i_serv;

	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		ping ( i_serv );
	}
}

void
ping ( int i_serv ) {
	udp_send ( serv_list[i_serv].name, PORT, "PING");
}

void
print_rechable_servers ( void ) {
	int i_serv;
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		if ( P[i_serv] == UP ) {
			log_info ("[RECHABLE] %s", serv_list[i_serv].name);
		}
	}
}

void
reset_servers ( void ) {
	int i_serv;
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		P[i_serv] = DOWN;
	}
}

void
reset_timers ( void ) {
	int i_serv;
	for ( i_serv = 0; i_serv < TOTAL_SERVERS; i_serv++ ) {
		T[i_serv] = 20;
	}
}
