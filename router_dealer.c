/* 
 * Operating Systems
 * Interprocess Communication
 *
 * ThatCT
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>    // for execlp
#include <mqueue.h>    // for mq


#include "settings.h"  
#include "messages.h"


static char mq_Req_queue [80];
static char mq_S1_queue [80];
static char mq_S2_queue [80];
static char mq_Rsp_queue [80];

static void create_client(pid_t client_ID)
{
	pid_t 	processID;
	/* Create client */
	processID = fork();
	if(processID < 0){
		perror("fork() failed");
		exit(1);
	}
	else{
		if(processID == 0){
			execlp("./client", "client", mq_Req_queue, NULL);
			perror("execlp()");
		}
		client_ID = processID;
		fprintf(stderr, "		Client %d has been finished\n", client_ID);
	}
}

static void create_workers(pid_t workers_IDs[])
{
	int 	i;
	pid_t 	processID;
	
	/*		Create service 1 workers		*/
	for(i = 0; i < N_SERV1; i++){
		processID = fork();
		if (processID < 0){
			perror("fork() failed");
			exit (1);
		}
		else if (processID == 0){
			execlp("./worker_s1", "worker_s1", mq_S1_queue, mq_Rsp_queue, NULL);
			perror("execlp() failed");
		}
		workers_IDs[i] = processID;
		fprintf(stderr, "		service 1 worker %d has been finished\n", workers_IDs[i]);
	}
	/*		Create service 2 workers		*/
	for(i = 0; i < N_SERV2; i++){
		processID = fork();
		if (processID < 0){
			perror("fork() failed");
			exit (1);
		}
		else if (processID == 0){
			execlp("./worker_s2", "worker_s2", mq_S2_queue, mq_Rsp_queue, NULL);
			perror("execlp() failed");
		}
		workers_IDs[i+N_SERV1] = processID;
		fprintf(stderr, "		service 2 worker %d has been finished\n", workers_IDs[i+N_SERV1]);
	}
}

static void getattr(mqd_t mq_fd)
{
	/* Check if message queues are correct	*/
	struct mq_attr  attr;
	int rtnval	=   mq_getattr(mq_fd, &attr);
	if(rtnval == -1){
		perror ("mq_getattr() failed");
		exit (1);
	}
	fprintf(stderr, "%d: mqdes=%d max=%ld size=%ld nrof=%ld\n", getpid(), mq_fd, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
}


int main (int argc, char * argv[])
{
	if (argc != 1)
		fprintf (stderr, "%s: invalid arguments\n", argv[0]);

	struct mq_attr	attr;
	MQ_REQ_MESSAGE	req;
	MQ_S1_MESSAGE 	s1;
	MQ_S2_MESSAGE 	s2;
	MQ_RSP_MESSAGE	rsp;
	mqd_t		mq_Req;
	mqd_t		mq_S1;
	mqd_t		mq_S2;
	mqd_t		mq_Rsp;
	pid_t		workers_IDs[N_SERV1+N_SERV2];
	pid_t 		client_ID;
	
	sprintf(mq_Req_queue, "/mq_Req_queue_%s_%d", NAME, getpid());
	sprintf(mq_S1_queue, "/mq_S1_queue_%s_%d", NAME, getpid());	
	sprintf(mq_S2_queue, "/mq_S2_queue_request_%s_%d", NAME, getpid());
	sprintf(mq_Rsp_queue, "/mq_Rsp_queue_response_%s_%d", NAME, getpid());
	
	attr.mq_curmsgs = 0;
	attr.mq_maxmsg 	= MQ_MAX_MESSAGES;
	attr.mq_msgsize	= sizeof(MQ_REQ_MESSAGE);
	mq_Req			= mq_open(mq_Req_queue, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
	if(mq_Req < 0){
		perror("error opening request queue");
		exit(1);
	}
	attr.mq_maxmsg 	= MQ_MAX_MESSAGES;
	attr.mq_msgsize	= sizeof(MQ_S1_MESSAGE);
	mq_S1			= mq_open(mq_S1_queue, O_WRONLY | O_CREAT | O_EXCL, 0666, &attr);
	if(mq_S1 < 0){
		perror("error opening S1 queue");
		exit(1);
	}
	attr.mq_maxmsg 	= MQ_MAX_MESSAGES;
	attr.mq_msgsize	= sizeof(MQ_S2_MESSAGE);
	mq_S2			= mq_open(mq_S2_queue, O_WRONLY | O_CREAT | O_EXCL, 0666, &attr);
	if(mq_S2 < 0){
		perror("error opening S2 queue");
		exit(1);
	}
	attr.mq_maxmsg 	= MQ_MAX_MESSAGES;
	attr.mq_msgsize	= sizeof(MQ_RSP_MESSAGE);
	mq_Rsp			= mq_open(mq_Rsp_queue, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
	if(mq_Rsp < 0){
		perror("error opening response queue");
		exit(1);
	}
	
	getattr(mq_Req);	// check message request queue
	getattr(mq_S1);		// check message s1 request queue
	getattr(mq_S2);		// check message s2 request queue
	getattr(mq_Rsp);	// check response queue
	
	create_client(client_ID);	    // create client process
	create_workers(workers_IDs);	// create worker processes
	
	int intres[80];
	int intreq[80];
	int temp = 0;
	int curmsgs = 0;
    
	while(true){				// while there are messages do x
		fprintf(stderr, "		router: receiving from client...\n");
		mq_receive(mq_Req, (char *) &req, sizeof (req), 0);		// receive message from client
		fprintf(stderr, "		router: received from client: %d, %d, %d\n", req.Req_ID, req.Ser_ID, req.Data);
		fprintf(stderr, "		before req_ID = %d \n", req.Req_ID);
		
        /* get out the while loop if request id is -1 */
		if(req.Req_ID < 0){
			int close1;
			for(close1 = 0; close1 < N_SERV1; close1++){	// For the amount of service 1 workers send them all the end signal 
				s1.Req_ID = req.Req_ID;
				s1.Data = 0;
				fprintf(stderr, "		router ending s1 with: %d %d\n", s1.Req_ID, s1.Data);
				mq_send (mq_S1, (char *) &s1, sizeof (s1), 0);		// send end signal to worker s1
			}
			int close2;
			for(close2 = 0; close2 < N_SERV2; close2++){	// For the amount of service 2 workers send them all the end signal
				s2.Req_ID = req.Req_ID;
				s2.Data = 0;
				fprintf(stderr, "		router ending s2 with: %d %d\n", s2.Req_ID, s2.Data);
				mq_send (mq_S2, (char *) &s2, sizeof (s2), 0);		// send end signal to worker s2
			}
			break;
		}
		else if(req.Ser_ID == 1){
			s1.Req_ID = req.Req_ID;
			s1.Data = req.Data;
			fprintf(stderr, "		router sending to s1: %d %d\n", s1.Req_ID, s1.Data);
            mq_send (mq_S1, (char *) &s1, sizeof (s1), 0);		// send message to worker s1
            curmsgs++;
		}
		else if(req.Ser_ID == 2){
			s2.Req_ID = req.Req_ID;
			s2.Data = req.Data;
			fprintf(stderr, "		router sending to s2: %d %d\n", s2.Req_ID, s2.Data);
            mq_send (mq_S2, (char *) &s2, sizeof (s2), 0);		// send message to worker s1
            curmsgs++;
		}

		while(curmsgs > 0){
			fprintf(stderr, "		router: receiving from response...  curmsgs: %d\n", curmsgs);
			mq_receive(mq_Rsp, (char *) &rsp, sizeof (rsp), 0);
			fprintf(stderr, "		router: received from response: %d, %d\n", rsp.Req_ID, rsp.Result);
			
			intres[temp] = rsp.Result;
			intreq[temp] = rsp.Req_ID;
			curmsgs--;
			temp++;
		}

		if(req.Req_ID < 0)
			break;
	}
	
	/* sort the output of the response queue to be printed correctly */
	int spot, len;
	for(len = 1; len <= temp; len++)
		for(spot = 0; spot < temp; spot++)
			if(intreq[spot] == len)
				fprintf(stdout, "%d -> %d\n", intreq[spot], intres[spot]);

    /* close all queues */
	int close_Req = mq_close(mq_Req);		// close request message queue
	if(close_Req < 0){					    // check for errors
		perror("failed to close mq_request queue");
		exit(1);
	}
	int close_S1 = mq_close(mq_S1);		    // close s1 message queue
	if(close_S1 < 0){					    // check for errors
		perror("failed to close mq_response queue");
		exit(1);
	}
	int close_S2 = mq_close(mq_S2);		    // close s2 message queue
	if(close_S2 < 0){					    // check for errors
		perror("failed to close mq_response queue");
		exit(1);
	}
	int close_Rsp = mq_close(mq_Rsp);		// close response message queue
	if(close_Rsp < 0){					    // check for errors
		perror("failed to close mq_response queue");
		exit(1);
	}

	int unlink_Req = mq_unlink(mq_Req_queue);	// unlink request message queue
	if(unlink_Req < 0){						    // check for errors
		perror("failed to close mq_router_request queue");
		exit(1);
	}
	int unlink_S1 = mq_unlink(mq_S1_queue); // unlink request message queue
	if(unlink_S1 < 0){						// check for errors
		perror("failed to close mq_worker_response queue");
		exit(1);
	}
	int unlink_S2 = mq_unlink(mq_S2_queue); // unlink request message queue
	if(unlink_S2 < 0){						// check for errors
		perror("failed to close mq_worker_response queue");
		exit(1);
	}
	int unlink_Rsp = mq_unlink(mq_Rsp_queue);	// unlink request message queue
	if(unlink_Rsp < 0){						    // check for errors
		perror("failed to close mq_worker_response queue");
		exit(1);
	}
	return (0);
}
