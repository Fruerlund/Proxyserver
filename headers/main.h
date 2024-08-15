#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <poll.h>

#define MAX_CONN 100

/**
struct pollfd {
    int   fd;          file descriptor 
    short events;      requested events 
    short revents;     returned events 
};
 */
typedef struct conn_h {

    struct pollfd connfd[MAX_CONN]; //array
    int number_connections;

} conn_h;


/**
 * @brief 
 * 
 */
typedef struct listener_h {

    struct conn_h CLIENTCONNECTIONS;
    struct conn_h TARGETCONNECTIONS;
    int pool;

} listener_h;

/**
 * @brief 
 * 1) CONNECT
		+----+----+----+----+----+----+----+----+----+----+....+----+
		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
		+----+----+----+----+----+----+----+----+----+----+....+----+
 #    byt 1    1      2              4           variable       1
 */

#endif


