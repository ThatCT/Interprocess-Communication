/* 
 * Operating Systems
 * Interprocess Communication
 *
 * Contains definitions of message data structures
 *
 */

#ifndef MESSAGES_H
#define MESSAGES_H


/* define message data structures for each queue */
#define NAME			"ThatCT"
int cancelled1 = 0;
int cancelled2 = 0;

typedef struct
{
	int                    	Req_ID;
	int			Ser_ID;
	int                  	Data;
} MQ_REQ_MESSAGE;

typedef struct
{
	int                  	Req_ID;
	int                  	Data;
} MQ_S1_MESSAGE;


typedef struct
{
	int                   	Req_ID;
	int	           	Data;
} MQ_S2_MESSAGE;


typedef struct
{
	int                	Req_ID;
	int                	Result;
} MQ_RSP_MESSAGE;


#endif
