#include "../headers/protocol.h"


void printRequest(sock4_connect *conn) {

    if(conn->userid != NULL) {
        printf("\nSOCK4_CONNECT\nIP: %s\nPORT: %d\nVERSION: %d\nCMD: %d\nUserID: %s\n", conn->dip, conn->dport, conn->version, conn->cmd, conn->userid);
    }
    else {
        printf("\nSOCK4_CONNECT\nIP: %s\nPORT: %d\nVERSION: %d\nCMD: %d\n", conn->dip, conn->dport, conn->version, conn->cmd);
    }

}

void printReply(sock4_reply *reply) {

    printf("\nSOCK4_REPLY\nIP: %d\nPORT: %d\nVERSION: %d\nREPLY: %d\n", reply->dip, reply->dport, reply->version, reply->reply);

}

void printBuffer(char *buffer, int len) {

    int max_chars = 16;
    for(int i = 0; i < len; i++) {
        printf( "0x%x ", (uint8_t)buffer[i] );
        if( (i+1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}



/**
 * @brief 
 * 
 */
int readString(struct sock4_connect *sock4_connect, char *BUFFER) {

    unsigned char *p = (char *) malloc( sizeof(char) *  255);
    memset(p, '\0', 255);
    int index = 0;
    for(;;) {
        if( (BUFFER[index] == '\x00')) {
            break;
        }
        p[index] = BUFFER[index];
        index++;
    }

    p[index] = '\x0';

    sock4_connect->userid = p;

    return index;
}



/**
 * @brief 
 * 
 * @param BUFFER 
 * @param index 
 * @param count 
 * @return uint32_t 
 */
uint32_t parseIP(uint8_t *BUFFER, int index, int count) {
    
    /*
    142.250.186.67 <-> 8e.fa.ba.43 (0x8efaba43) (2398796355)
    [0x8e, 0xfa, 0xba, 0x43]
    */

    uint32_t x = (uint32_t)BUFFER[4] + ((uint32_t)BUFFER[5] << 8) + ((uint32_t)BUFFER[6] << 16) + ((uint32_t)BUFFER[7] << 24);

    return (uint32_t) x;
}


//========================================= PIPING =========================================//

/**
 * @brief 
 * 
 * @param listener 
 * @param client 
 * @param target 
 */
void setupPipe(struct listener_h *listener, int client, int target) {

    // https://linux.die.net/man/2/sendfile
    // https://linux.die.net/man/2/splice

    listener->CLIENTCONNECTIONS.connfd[client % MAX_CONN].fd = client;
    listener->CLIENTCONNECTIONS.connfd[client % MAX_CONN].events = POLLIN;

    listener->TARGETCONNECTIONS.connfd[client % MAX_CONN].fd = target;
    listener->TARGETCONNECTIONS.connfd[client % MAX_CONN].events = POLLIN;

    listener->CLIENTCONNECTIONS.number_connections++;
    listener->TARGETCONNECTIONS.number_connections++;


}



//========================================= REQUEST/REPLY HANDLING =========================================//

/**
 * @brief 
 * 
 * @param connect 
 * @param CD 
 * @param fd 
 */
void sendERROR(struct sock4_connect *connect, int CD, int fd) {

    char *data = makeReply(connect, CD);
    int DATA_TO_SEND = sizeof(sock4_reply);
    printBuffer(data, DATA_TO_SEND);
    send(fd, data, DATA_TO_SEND, 0);
    close(fd);

}



/**
 * @brief 
 * 
 * @param sock4_connect 
 */
int resolveHost(struct sock4_connect * sock4_connect, char *BUFFER) {

    // Read until NULLBYTE

    /*
    struct hostent {
    char  *h_name;             official name of host 
    char **h_aliases;          alias list 
    int    h_addrtype;         host address type 
    int    h_length;           length of address 
    char **h_addr_list;        list of addresses 
    }
    #define h_addr h_addr_list[0] /* for backward compatibility */
    
    
    struct hostent *host;
    host = gethostbyname(BUFFER);

    if(host != NULL) {
    sock4_connect->dip = inet_ntoa( *(struct in_addr *)host->h_addr_list[0]);
    }

    else {
        printf("ERROR Resolving hostname: %d\n", h_errno);
    }
    return 0;

}



/**
 * @brief 
 * 
 * @param sock4_connect 
 * @return int 
 */
int attemptRequest(struct sock4_connect * sock4_connect) {

    int c = -1;
    
    struct sockaddr_in serv_addr;

    if ( ( c = create_socket() ) == -1 ) {
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(sock4_connect->dport);
       
    if(inet_pton(AF_INET, sock4_connect->dip, &serv_addr.sin_addr)<=0) 
    {
        printf("Invalid address / Address not supported \n");
        return -1;
    }
   
    if (connect(c, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed \n");
        return -1;
    }
  
    return c;
}



/**
 * @brief 
 * 
 * @param listener 
 * @param clientfd 
 * @return int 
 */
char *makeReply(struct sock4_connect * sock4_connect, uint8_t CD) {

    struct sock4_reply sock4_reply;

    unsigned char *buffer = (char *) malloc (sizeof(sock4_reply));
    memset(buffer, 0, sizeof(sock4_reply));
    
    /*
    0x4 0x1 0x1 0xbb 0x8e 0xfa 0xba 0x43 0x0
    */

    sock4_reply.version = 0;      
    sock4_reply.reply = CD;                             
    sock4_reply.dport = 0x0; //sock4_connect->dport;

    sock4_reply.dip = 0x0; //net_addr(sock4_connect->dip);           
    
    printReply(&sock4_reply);

    memcpy(buffer, (const unsigned char*)&sock4_reply, sizeof(sock4_reply));
    
    /*
    sock4_reply.dip = (char *) malloc (sizeof(char) * 17);
    int index = 0;
    for(;;) {
        
        if(sock4_connect->dip[index] == '\x0') {
            break;
        }
        buffer[4+index] = sock4_connect->dip[index];   
        index++;
    }
    index++;
    buffer[4+index] = '\x0';
    */

    return buffer;
}



/**
 * @brief 
 * 
 * @param sock4_connect 
 * @param BUFFER 
 * @return int 
 */
int readRequest(struct sock4_connect *sock4_connect, char *BUFFER, int RECIEVED_BYTES) {

    int userID_len = 0;

    sock4_connect->version = BUFFER[PACKET_VN];
    sock4_connect->cmd = BUFFER[PACKET_CD];

    // 443 [0x01bb]
    // 0x01 << 8        --> 0x0001
    // 0x0001 | 0xbb     --> 0xbb01
    uint8_t port = BUFFER[P2];
    uint16_t dport = port | (BUFFER[P1] << 8);
    sock4_connect->dport = dport;

    uint32_t ip = parseIP(BUFFER, 4, 4);
    struct in_addr ip_addr;
    ip_addr.s_addr = ip;
    sock4_connect->dip = inet_ntoa(ip_addr);

    // Read UserID of maximum(256 bytes), until NULL terminator
    if(BUFFER[8] != '\x00') {
        userID_len = readString(sock4_connect, &BUFFER[8]);
    }

    return userID_len;
}
            


/**
 * @brief 
 * 
 * @param listener 
 * @param clientfd 
 * @return int 
 */
int handleRequest(struct listener_h *listener, int clientfd) {

    unsigned char BUFFER[256];
    struct sock4_connect sock4_connect;
    int BYTES_SENT = 0;

    memset(BUFFER, 0, sizeof(BUFFER));
    
    int RECIEVED_BYTES = recv(clientfd, BUFFER, sizeof(BUFFER), 0);

    // If socks4a we use UserID_Length to index into buffer to find domainname
    int UserID_Length = readRequest(&sock4_connect, BUFFER, RECIEVED_BYTES);
    printRequest(&sock4_connect);
    printBuffer(BUFFER, RECIEVED_BYTES);

    if(sock4_connect.cmd == 1) {
        
        if(sock4_connect.dport == 443) {
            printf("ERROR. HTTPS not SUPPORTED!\n");
            /*
            sendERROR(&sock4_connect, 91, clientfd);
            return -1;
            */
        }

        // Check if client wants server to resolve domainname.
        struct in_addr ip_addr; 
        ip_addr.s_addr = inet_network(sock4_connect.dip); 
        
        if ( (ip_addr.s_addr > 0x0) && (ip_addr.s_addr <= 0xff)) {
            if ( resolveHost(&sock4_connect, &BUFFER[9 + UserID_Length] ) < 0 ) {
                
                // Cannot resolve requested domainname
                sendERROR(&sock4_connect, 91, clientfd);
                return -1;

            }
        }
        
        // Proxy Server should make HTTPS request if target port is 443.

        // Attempt to create a connection to the requested IP:PORT
        int new_connection = -1;
        if(  (new_connection = attemptRequest(&sock4_connect) ) > 0) {

            // Connection succeeded
            setupPipe(listener, clientfd, new_connection);
            char *data = makeReply(&sock4_connect, REQ_GRANT);
            int DATA_TO_SEND = sizeof(sock4_reply);
            printBuffer(data, DATA_TO_SEND);
            send(clientfd, data, DATA_TO_SEND, 0);
            return 0;

        } 
    }

    sendERROR(&sock4_connect, REQ_REJFAIL, clientfd);
    return -1;
    
}
