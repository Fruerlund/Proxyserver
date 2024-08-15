#include "../headers/main.h"


/**
 * @brief 
 * 
 * @param listener 
 * @return int 
 */

int initlistener(struct listener_h *listener) {


    // Set all POOL FDs to -1.
    for(int i = 0; i < MAX_CONN; i++) {

        listener->CLIENTCONNECTIONS.connfd[i].fd = -1;
        listener->TARGETCONNECTIONS.connfd[i].fd = -1;
        listener->TARGETCONNECTIONS.connfd[i].events = 0;
        listener->TARGETCONNECTIONS.connfd[i].revents = 0;

    }
    
    listener->CLIENTCONNECTIONS.number_connections = 0;
    listener->TARGETCONNECTIONS.number_connections = 0;

    return 0;
}



/**
 * 
 */ 
int main(int argc, char **argv) {


    printf("Starting...\n");

    if(argc < 1) {
        printf("Please supply a proper PORT number!\n");
        return -1;
    }

    int PORT = atoi(argv[1]);

    struct listener_h listener;
    initlistener(&listener);

    startListener(&listener, PORT);

    return 0;
    
}