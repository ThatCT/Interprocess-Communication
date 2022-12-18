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
#include "service1.h"

static void rsleep (int t);


int main (int argc, char * argv[]){
	MQ_S1_MESSAGE 	s1;
	MQ_RSP_MESSAGE	rsp;
	mqd_t 		mq_s1;
	mqd_t		mq_rsp;
    	
	mq_s1 = mq_open(argv[1], O_RDONLY);		// open message queue for s1 request
	mq_rsp = mq_open(argv[2], O_WRONLY);	// open message queue for response
	if(mq_s1 < 0)							// check for errors
		perror("error opening farmer request message queue in worker1");
	if(mq_rsp < 0)						    // check for errors
		perror("error opening farmer request message queue in worker1");
	
    while(true){							// while there are jobs
        fprintf(stderr, "						worker1: receiving from router...\n");
        ssize_t received = mq_receive(mq_s1, (char *) &s1, sizeof(s1), 0);	//Receive message from router
        if(received < 0)
            exit(77);
        fprintf(stderr, "						worker1: received from router: %d, %d\n", s1.Req_ID, s1.Data);
        
        if(s1.Req_ID < 0){					        // break the loop if no messages to be received
            fprintf(stderr, "						Break the loop of worker 1 please thank you\n");
            break;		
        }
        int ID      = s1.Req_ID;
        int Data    = s1.Data;

        rsleep(1000);						// sleep for random amount of time
        
        int result  = service(Data);
        rsp.Result  = result;
        rsp.Req_ID  = ID;
        
        fprintf(stderr, "						worker1 sending to router: %d, %d	\n", rsp.Req_ID, rsp.Result);
        int sent    = mq_send(mq_rsp, (char *) &rsp, sizeof(rsp), 0);	// send message to router
        if(sent < 0)
            exit(78);
        fprintf(stderr, "						worker1 sent to router!\n");
	}

	int close_s1 = mq_close(mq_s1);				// close message queue mq_s1
	int close_rsp = mq_close(mq_rsp);			// close message queue mq_rsp
	if(close_s1 < 0)						// check for errors
		perror("failed to close mq_s1 in worker1");		
	if(close_rsp < 0)						// check for errors
		perror("failed to close mq_rsp in worker1");		
	
	return (0);
}

static void rsleep (int t){
	static bool first_call = true;

	if (first_call == true){
		srandom (time (NULL) % getpid ());
		first_call = false;
	}
	usleep (random() % t);
}
