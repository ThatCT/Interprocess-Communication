/* 
 * Operating Systems
 * Interprocess Communication
 *
 * Contains definitions that are used by the clients
 *
 */

#ifndef REQUEST_H
#define REQUEST_H

// data structure containing the information of one request
typedef struct {
	int job, data, service;
} Request;



// Error codes for getNextRequest(...)
#define NO_ERR	0
#define NO_REQ	-1
 

// Places the information of the next request in the parameters sent by reference.
// Returns NO_REQ if there is no request to make.
// Returns NO_ERR otherwise.
int getNextRequest(int* jobID, int* data, int* serviceID);


#endif

