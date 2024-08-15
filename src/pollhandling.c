#include "../headers/protocol.h"
#define BUFF_SIZE 1024

/**
 * @brief 
 * 
 * @param listener 
 * @param connfd 
 * @param timeout 
 * @param buffer 
 */
void poll_CLIENT_TO_TARGET(struct listener_h *listener, int timeout, char *buffer) {

    int rc = poll(listener->CLIENTCONNECTIONS.connfd, MAX_CONN, timeout);

    if(rc == 0) { // TIMEOUT
        return;
    }
    if(rc < 0) {
        return;  // ERROR
    }

    for(int i = 0; i < MAX_CONN; i++) {

        struct pollfd CLIENT = listener->CLIENTCONNECTIONS.connfd[i];

        if(CLIENT.revents == 0) {
            continue;
        }
        if(CLIENT.revents != POLLIN) {
            break;
        }

        /*
        Read input from socket
        */
        rc = recv(CLIENT.fd, buffer, 1024, 0);
        if(rc < 0) {
            break;
        }

        struct pollfd TARGET = listener->TARGETCONNECTIONS.connfd[i];
        if(rc > 1) {
            write(TARGET.fd, buffer, rc);
        }
        
        if(rc == 0) {

            close(CLIENT.fd);
            listener->CLIENTCONNECTIONS.connfd[i].fd = -1;
            
            close(TARGET.fd);
            listener->TARGETCONNECTIONS.connfd[i].fd = -1;

            listener->CLIENTCONNECTIONS.number_connections--;
            listener->TARGETCONNECTIONS.number_connections--;


            break;
        }

        printf("< (C: %d) <--> (T: %d) transfered %d byte(s)\n", CLIENT.fd, TARGET.fd, rc);
        printBuffer(buffer, rc);
        memset(buffer, 0, 1024);
    }
}


/**
 * @brief 
 * 
 * @param listener 
 * @param connfd 
 * @param timeout 
 * @param buffer 
 */
void poll_TARGET_TO_CLIENT(struct listener_h *listener, int timeout, char *buffer) {

    int rc = poll(listener->TARGETCONNECTIONS.connfd, MAX_CONN, timeout);

    if(rc == 0) { // TIMEOUT
        return;
    }
    if(rc < 0) {
        return;  // ERROR
    }

    for(int i = 0; i < MAX_CONN; i++) {

        struct pollfd TARGET = listener->TARGETCONNECTIONS.connfd[i];

        if(TARGET.revents == 0) {
            continue;
        }
        if(TARGET.revents != POLLIN) {  
            break;
        }

        /*
        Read input from socket
        */
        
        rc = recv(TARGET.fd, buffer, 1024, 0);

        if(rc < 0) {
            break;
        }

        struct pollfd CLIENT = listener->CLIENTCONNECTIONS.connfd[i];

        if(rc > 1) {
            write(CLIENT.fd, buffer, rc);
        }
        
        if(rc == 0) {

            printf("Connection closed for ID: %d\n", TARGET.fd);
            close(TARGET.fd);
            listener->TARGETCONNECTIONS.connfd[i].fd = -1;
            
            close(CLIENT.fd);
            listener->CLIENTCONNECTIONS.connfd[i].fd = -1;

            listener->CLIENTCONNECTIONS.number_connections--;
            listener->TARGETCONNECTIONS.number_connections--;

            break;
        }

        printf("< (T: %d) <--> (C: %d) transfered %d byte(s)\n", TARGET.fd, CLIENT.fd, rc);
        printBuffer(buffer, rc);
        memset(buffer, 0, 1024);
    }
}



/**
 * @brief 
 * 
 * @param listener 
 * @return void* 
 */

void *pooling(struct listener_h *listener) {

    char BUFFER[BUFF_SIZE]; 
    memset(BUFFER, 0, BUFF_SIZE);

    listener->pool = 1;
    int rc = 0;
    int timeout = 1;

    printf("Started poll()\n");

    while(1) {

        if(listener->pool == 0) {
            // Destroy thread and shut down connections!
            break;
        }

        poll_CLIENT_TO_TARGET(listener, timeout, BUFFER);
        poll_TARGET_TO_CLIENT(listener, timeout, BUFFER);

    }

    listener->pool = 0;
}
