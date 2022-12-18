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
#include "service2.h"

static void rsleep (int t);


int main (int argc, char * argv[]){
	MQ_S2_MESSAGE 	s2;
	MQ_RSP_MESSAGE	rsp;
	mqd_t 		mq_s2;
	mqd_t		mq_rsp;
    	
	mq_s2 = mq_open(argv[1], O_RDONLY);
	mq_rsp = mq_open(argv[2], O_WRONLY);
	if(mq_s2 < 0){							//Check for errors
		perror("error opening s2 request message queue in worker2");
	}
	if(mq_rsp < 0){						//Check for errors
		perror("error opening response request message queue in worker2");
	}
	
    	//  * repeatingly:
	//      - read from the SX message queue the new job to do
	//      - wait a random amount of time (e.g. rsleep(10000);)
	//      - do the job 
	//      - write the results to the Rep message queue
	//    until there are no more tasks to do
	
    while(true){
        fprintf(stderr, "				worker2: receiving from router...\n");
        ssize_t received = mq_receive(mq_s2, (char *) &s2, sizeof(s2), 0);	//Receive message from router
        if(received < 0)					// check for error
            exit(77);
        fprintf(stderr, "				worker2: received from router: %d, %d\n", s2.Req_ID, s2.Data);
        
        if(s2.Req_ID < 0){					// break the loop if no messages to be received
            fprintf(stderr, "				Break the loop of worker 2 \n");
            break;		
        }
        int ID      = s2.Req_ID;
        int Data    = s2.Data;

        rsleep(10000);						//sleep for random amount of time
        
        int result  = service(Data);
        rsp.Result  = result;
        rsp.Req_ID  = ID;
        
        fprintf(stderr, "				worker2 sending to router: %d, %d\n", rsp.Req_ID, rsp.Result);
        int sent    = mq_send(mq_rsp, (char *) &rsp, sizeof(rsp), 0);	//Send message to router
        if(sent < 0)
            exit(77);
        fprintf(stderr, "				worker2 sent to router!\n");
	}

	int close_s2 = mq_close(mq_s2);				// close message queue mq_s2
	int close_rsp = mq_close(mq_rsp);			// close message queue mq_rsp
	
	if(close_s2 < 0)						// check for errors
		perror("failed to close mq_s2 in worker2");
	if(close_rsp < 0)						// check for errors
		perror("failed to close mq_rsp in worker2");
	
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
