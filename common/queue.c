/*
 * File: queue.c
 *
 * This file contains all functions to manupalate queue
 */

#include "common.h"

/*
 * Insert element (node) to
 * top of queue
 */
void
insert_queue ( queue **q, char *from, char *text ) {
	queue *traverse = *q;
	queue *temp = malloc ( sizeof(queue) );
	// Creating the temp queue
	strncpy ( temp->host, from, HOST_SIZE );
	strncpy ( temp->data, text, BUF_LEN );
	temp->next = NULL;
	if ( *q == NULL ) {
		//printf("test");
		*q = temp;
		return;
	}
	while ( traverse->next != NULL ) {
		traverse = traverse->next;
	}
	traverse->next = temp;
}

/*
 * Remove node from top of queue
 * Return type is queue
 */
queue *
remove_queue ( queue **q ) {
	if ( is_queue_empty(*q) )
		return NULL;
	else {
		queue *temp = *q;
		*q = (*q)->next;
		return temp;
	}
}

/*
 * Check if queue is empty!
 */
int
is_queue_empty ( queue *q ) {
	if ( q == NULL )
		return 1;
	else
		return 0;
}

/*
 * Dummy print function
 * to check if queue works...
 */
void
print_queue ( queue *q ) {
	if ( is_queue_empty(q) )
		return;
	queue *temp = q;
	while ( temp != NULL ) {
		fprintf (stdout, "[QUEUE] %s: %s\n", temp->host, temp->data );
		temp = temp->next;
	}
}
