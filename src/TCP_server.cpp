#include <iostream>
#include <string>
#include <thread>
#include <netinet/in.h>     ///> sockaddr_in
#include <string.h>         ///> bzero()
#include <sys/time.h>       ///> gettimeofday()
#include <cerrno>
#include <unistd.h>

#include <Logger.hpp>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SERVER                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void TCP_server_func(){

    ///> setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
        /* Style */
    const string side_name {"\033[1;36mSERVER\033[0;0m :"};
    
        /* Log */
    LOG_INIT_COUT();
    log(LOG_DONE) << side_name << "is alive\n";

        /* Sys */
    const char* TXT_SAMPLE  = "AAAAAAAA";
    int         count       {0};
    const int   port_id     = 12345;                                        ///> port id
    const int   msg_max_len = 4096;
    char        msg[msg_max_len];                                           ///> communication buffer (send/receive)
    struct sockaddr_in server_socket;
    struct sockaddr_in new_socket;                                          ///> Client
    socklen_t new_socket_size = sizeof(new_socket);                         ///> new add-s to connect with the client
    
        /* setup socket and connection tools */
    bzero((char*) &server_socket, sizeof(server_socket));                   ///> explicitly erase data in memory of 'server_socket' struct
    bzero((char*) &new_socket, sizeof(new_socket));
    server_socket.sin_family = AF_INET;                                     ///> set protocol = IPv4 Internet protocols
    server_socket.sin_addr.s_addr = htonl(INADDR_ANY);                      ///> internet add-s = convert 'uns. int' byte order to 'network' byte order for 'any add-s'
    server_socket.sin_port = htons(port_id);                                ///> set ports id in 'network' byte order

        /* 1. open stream oriented socket with internet address */
        /* 2. also keep track of the socket descriptor */
    const int server_sd = socket(AF_INET, SOCK_STREAM, 0);                  ///> create a socket
    if(server_sd < 0){
        log(LOG_ERR) << side_name << "socket wasn`t established!\n";
        log(LOG_ERR) << side_name << "errno '" << strerror(errno) << "'\n";
        exit(1);
    }

        /* bind the socket to its local add */
    const int bind_status = bind(server_sd, (struct sockaddr*) &server_socket, sizeof(server_socket));
    if(bind_status < 0){
        log(LOG_ERR) << side_name << "binding of socket with local add-s wasn`t done!\n";
        log(LOG_ERR) << side_name << "errno '" << strerror(errno) << "'\n";
        exit(1);
    }

    log(LOG_INFO) << side_name << "wait for a client to connect\n";
        
        /* waiting for connection for 5 sec */
    listen(server_sd, 5);

        /* receive a request from client */

        /* accept/create a new socket descriptor to handle the connection */
    log(LOG_INFO) << side_name << "accept client call...\n";
    const int new_sd = accept(server_sd, (struct sockaddr*) &new_socket, &new_socket_size);
    if(new_sd < 0){
        log(LOG_ERR) << side_name << "request from client wasn`t accepted !\n";
        log(LOG_ERR) << side_name << "errno '" << strerror(errno) << "'\n";
        exit(1);
    }

    log(LOG_DONE) << side_name << "connected with the client!\n";

        /* print */
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    int bytes_read {0}, bytes_written {0};

    ///> loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    while(true){
            /* clear buffer */
        memset(&msg, 0, sizeof(msg));
        bytes_read += recv(new_sd, (char*) &msg, sizeof(msg), 0);

        log(LOG_DONE) << side_name << "recv << " << msg << "\n";
        
        if(!strcmp(msg, TXT_SAMPLE)){
            count++;
        }

            /* clear buffer */
        memset(&msg, 0, sizeof(msg));

        if(count <= 10){
            strcpy(msg, TXT_SAMPLE);
                /* send msg to the client */
            bytes_written += send(new_sd, (char*) &msg, strlen(msg), 0);
            log(LOG_DONE) << side_name << "send >> " << msg << "\n";
        }
        else{
            strcpy(msg, "exit");
                /* send msg to the client */
            bytes_written += send(new_sd, (char*) &msg, strlen(msg), 0);
            log(LOG_DONE) << side_name << "send >> " << msg << "\n";
            break;
        }
    }
    ///> end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|

    this_thread::sleep_for(1000ms);

        /* close all dependencies */
    gettimeofday(&end_time, NULL);
    close(new_sd);
    close(server_sd);

    log(LOG_INFO) << side_name << "   ******** Server Session ********\n";
    log(LOG_INFO) << side_name << "   | Bytes written: " << bytes_written << "\n";
    log(LOG_INFO) << side_name << "   | Bytes read:    " << bytes_read << "\n";
    log(LOG_INFO) << side_name << "   | Elapsed time:  " << (end_time.tv_sec - start_time.tv_sec) << " secs\n";
    
    log(LOG_INFO) << side_name << " killed\n";
}