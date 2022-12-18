/* 
 * Operating Systems
 * Interprocess Communication
 *
 * ThatCT
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>      // for perror()
#include <unistd.h>     // for getpid()
#include <mqueue.h>     // for mq-stuff
#include <time.h>       // for time()

#include "messages.h"
#include "request.h"

static void getattr(mqd_t mq_fd)
{
	/*		Check if message queues are correct	*/
	struct mq_attr 	attr;
	int rtnval	= 	mq_getattr (mq_fd, &attr);
	if(rtnval == -1)
	{
		perror ("mq_getattr() failed");
		exit (1);
	}
	fprintf (stderr, "%d: mqdes=%d max=%ld size=%ld nrof=%ld\n",getpid(), mq_fd, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
}

int main (int argc, char * argv[])
{
	MQ_REQ_MESSAGE	req_message;
	Request 	req;
	mqd_t		mq_client;
	struct mq_attr	attr;
	
	mq_client = mq_open(argv[1], O_WRONLY);				// open message queue for request
	if (mq_client < 0)									// check for errors
		perror("error opening farmer request message queue in worker");
	
	fprintf(stderr, "value for req.job = %d\n", req.job);		
	fprintf(stderr, "value for req.service = %d\n", req.service);
	fprintf(stderr, "value for req.data = %d\n", req.data);
	
	int request;										// initialize values used in loop
	int count = 0;                                      // counts the number of sent messages
	int curmsgs;
	
	int rtnval	= 	mq_getattr (mq_client, &attr);
	if(rtnval == -1){
		perror ("mq_getattr() failed");
		exit (1);
	}
	
	while(true){										// while there are messages to be sent
        request = getNextRequest(&req.job, &req.data, &req.service);			// check if there is a next message
        fprintf(stderr, "Request check = %d\n", request);
        if(request < 0){							// if there is no request break the while loop
            fprintf(stderr, "Client ending router... 		\n");
            req_message.Req_ID = -1;
            req_message.Ser_ID = 0;
            req_message.Data = 0;
            mq_send(mq_client, (char*)&req_message, sizeof(req_message), 0);	//send error to router
            fprintf(stderr, "broke the client loop \n");
            break;
        }
                    
        req_message.Req_ID = req.job;
        req_message.Ser_ID = req.service;
        req_message.Data = req.data;
        
        int time = 10000;
        unsigned int prio = 100-count;
        fprintf(stderr, "Client sending... 		\n");
        mq_send(mq_client, (char*)&req_message, sizeof(req_message), 0);		// send correct message to router
        fprintf(stderr, "Send Count = %d \n", count);
        count++;
	}
	
	int close_req = mq_close(mq_client);	// close message queue for request
	if(close_req < 0)					    // check for errors
		perror("failed to close mq_client");

    return (0);
}
