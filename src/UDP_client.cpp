#include <iostream>
#include <thread>
#include <sys/time.h>       ///> gettimeofday()
#include <netinet/in.h>     ///> sockaddr_in
#include <string.h>         ///> bzero()
#include <unistd.h>

#include <Logger.hpp>

using namespace std;

//////////////////////////
// #define SINGLE_RUN
//////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SERVER                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void UDP_client_func(){

    ///> setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    const string    side_name           {"\033[1;33mCLIENT\033[0;0m :"};

    LOG_INIT_COUT();
    log(LOG_DONE) << side_name << "is alive\n";

    int             sockfd;
    const char*     TXT_SAMPLE          {"AAAAAAAA"};
    const int       port_id             {8080};
    const int       msg_max_len         {1024};
    char            msg[msg_max_len];
    int             bytes_read          {0};
    int             bytes_written       {0};
    struct timeval  start_time, end_time;
    struct sockaddr_in     client_socket;
    socklen_t       len;
   
    // Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd <= 0 ) {
        log(LOG_ERR) << side_name << "socket() wasn`t established! ERRNO '" << strerror(errno) << "'\n";
        exit(1);
    }
    else{
        log(LOG_DONE) << side_name << "socket() was established!\n";
    }

    memset(&client_socket, 0, sizeof(client_socket));
       
    // Filling server information
    client_socket.sin_family = AF_INET;
    client_socket.sin_port = htons(port_id);
    client_socket.sin_addr.s_addr = INADDR_ANY;
    
    gettimeofday(&start_time, NULL);

    ///> loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    
#if defined(SINGLE_RUN)
    bytes_written += sendto(sockfd, (const char *)TXT_SAMPLE, strlen(TXT_SAMPLE), MSG_CONFIRM, (const struct sockaddr *) &client_socket, sizeof(client_socket));
    log(LOG_DONE) << side_name << "send >> " << TXT_SAMPLE << "\n";
    len = sizeof(client_socket);
    bytes_read += recvfrom(sockfd, (char *)msg, msg_max_len, MSG_WAITALL, (struct sockaddr *) &client_socket, &len);
    log(LOG_DONE) << side_name << "recv << " << msg << "\n";
#else
    while(true){

        this_thread::sleep_for(1000ms);

        memset(&msg, 0, sizeof(msg));
        strcpy(msg, TXT_SAMPLE);
        
        bytes_written += sendto(sockfd, (const char *)msg, msg_max_len, MSG_CONFIRM, (const struct sockaddr *) &client_socket, sizeof(client_socket));
        log(LOG_DONE) << side_name << "send >> " << msg << "\n";

        memset(&msg, 0, sizeof(msg));
        len = sizeof(client_socket);

        bytes_read += recvfrom(sockfd, (char *)msg, msg_max_len, MSG_WAITALL, (struct sockaddr *) &client_socket, &len);
        log(LOG_DONE) << side_name << "recv << " << msg << "\n";

        if(!strcmp(msg, "exit")) {
            log(LOG_DONE) << side_name << "server has quite the session\n";
            break;
        }
    }
#endif
   
   ///> end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    gettimeofday(&end_time, NULL);

    this_thread::sleep_for(1000ms);

    close(sockfd);
    log(LOG_INFO) << side_name << "******** Server Session ********\n";
    log(LOG_INFO) << side_name << "   | Bytes written: " << bytes_written << "\n";
    log(LOG_INFO) << side_name << "   | Bytes read:    " << bytes_read << "\n";
    log(LOG_INFO) << side_name << "   | Elapsed time:  " << (end_time.tv_sec - start_time.tv_sec) << " secs\n";
    
    log(LOG_INFO) << side_name << "killed\n";
}
