
#ifndef PROTOCOL_H_   /* Include guard */
#define PROTOCOL_H_

#include "../headers/main.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>  
#include <fcntl.h>
#include <sys/types.h>

#define BUFF_SIZE 1024
#define PACKET_VN 0
#define PACKET_CD 1
#define P1 2
#define P2 3
#define REQ_GRANT 90
#define REQ_REJFAIL 91


typedef struct __attribute__((packed)) sock4_connect {

    uint8_t version;
    uint8_t cmd;
    uint16_t dport;
    char * dip;
    char *userid;

} sock4_connect;



/**
 * @brief 
 * 2) REPLY
 * 		+----+----+----+----+----+----+----+----+
		| VN | CD | DSTPORT |      DSTIP        |
		+----+----+----+----+----+----+----+----+
     byt:  1    1      2              4
 */
typedef struct __attribute__((packed)) sock4_reply {

    uint8_t  version;
    uint8_t reply;
    uint16_t dport;
    uint32_t dip;       

} sock4_reply;

void printRequest(sock4_connect *);
void printReply(sock4_reply *);
void printBuffer(char *, int );
int readString(sock4_connect *, char *);
uint32_t parseIP(uint8_t *, int, int);
void setupPipe(listener_h *, int, int);
void sendERROR(sock4_connect *, int, int);
int resolveHost(sock4_connect *, char *);
int attemptRequest(sock4_connect *);
char *makeReply(sock4_connect *, uint8_t);
int readRequest(sock4_connect *, char *, int);
int handleRequest(listener_h *, int);


#endif