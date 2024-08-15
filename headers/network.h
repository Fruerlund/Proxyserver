#ifndef NETWORK_H_
#define NETWORK_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>  
#include <fcntl.h>
#include <sys/types.h>

#include "../headers/main.h"
#include "../headers/pollhandling.h"

int create_socket(void);
void handleConnection(listener_h *, int);
int startListener(listener_h *, int);

#endif