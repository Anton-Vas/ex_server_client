#include <iostream>
#include <thread>
#include <sys/time.h>       ///> gettimeofday()
#include <netinet/in.h>     ///> sockaddr_in
#include <string.h>         ///> bzero()
#include <cerrno>
#include <unistd.h>

#include <Logger.hpp>

using namespace std;

//////////////////////////
// #define SINGLE_RUN
//////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SERVER                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int UDP_server_func() {
    const string side_name {"\033[1;36mSERVER\033[0;0m :"};
    
    LOG_INIT_COUT();
    log(LOG_DONE) << side_name << "is alive\n";
	
    int             sockfd;
    int             bind_status;
    const char*     TXT_SAMPLE          {"AAAAAAAA"};
    const int       port_id             {8080};
    const int       msg_max_len         {1024};
    char            msg[msg_max_len];
    int             bytes_read          {0};
    int             bytes_written       {0};
    int             count               {0};
    struct timeval  start_time, end_time;
    struct sockaddr_in     client_socket, server_socket;
    socklen_t       len;
	
	// Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd <= 0 ) {
        log(LOG_ERR) << side_name << "socket() wasn`t established! ERRNO '" << strerror(errno) << "'\n";
        exit(1);
	}
    else{
        log(LOG_DONE) << side_name << "socket() was established!\n";
    }
	
	memset(&server_socket, 0, sizeof(server_socket));
	memset(&client_socket, 0, sizeof(client_socket));
	
	// Filling server information
	server_socket.sin_family = AF_INET; // IPv4
	server_socket.sin_addr.s_addr = INADDR_ANY;
	server_socket.sin_port = htons(port_id);
	
	// Bind the socket with the server address
    bind_status = bind(sockfd, (const struct sockaddr *)&server_socket, sizeof(server_socket));
	if ( bind_status < 0 ){
        log(LOG_ERR) << side_name << "bind() wasn`t established! ERRNO '" << strerror(errno) << "'\n";
        exit(1);
	}
    else{
        log(LOG_DONE) << side_name << "bind() was established!\n";
    }

    ///> loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|

#if defined(SINGLE_RUN)
    len = sizeof(client_socket);
	bytes_written += recvfrom(sockfd, (char *)msg, msg_max_len, MSG_WAITALL, ( struct sockaddr *) &client_socket, &len);
    log(LOG_DONE) << side_name << "recv << " << msg << "\n";
	bytes_read += sendto(sockfd, (const char *)TXT_SAMPLE, strlen(TXT_SAMPLE), MSG_CONFIRM, (const struct sockaddr *) &client_socket, sizeof(client_socket));
    log(LOG_DONE) << side_name << "send >> " << TXT_SAMPLE << "\n";
#else
    ///> v2 LOOP
    while(true){
        
        memset(&msg, 0, sizeof(msg));
        len = sizeof(client_socket);

        bytes_read += recvfrom(sockfd, (char *)msg, msg_max_len, MSG_WAITALL, ( struct sockaddr *) &client_socket, &len);
        log(LOG_DONE) << side_name << "recv << " << msg << "\n";

        if(!strcmp(msg, TXT_SAMPLE)){
            count++;
        }

        memset(&msg, 0, sizeof(msg));

        if(count <= 10){
            strcpy(msg, TXT_SAMPLE);
            bytes_written += sendto(sockfd, (const char *)msg, msg_max_len, MSG_CONFIRM, (const struct sockaddr *) &client_socket, sizeof(client_socket));
            log(LOG_DONE) << side_name << "send >> " << msg << "\n";
        }
        else{
            strcpy(msg, "exit");
            bytes_written += sendto(sockfd, (const char *)msg, msg_max_len, MSG_CONFIRM, (const struct sockaddr *) &client_socket, sizeof(client_socket));
            log(LOG_DONE) << side_name << "send >> " << msg << "\n";
            break;
        }
    }
#endif

    ///> end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    gettimeofday(&end_time, NULL);

    this_thread::sleep_for(500ms);

    close(sockfd);

    log(LOG_INFO) << side_name << "******** Server Session ********\n";
    log(LOG_INFO) << side_name << "   | Bytes written: " << bytes_written << "\n";
    log(LOG_INFO) << side_name << "   | Bytes read:    " << bytes_read << "\n";
    log(LOG_INFO) << side_name << "   | Elapsed time:  " << (end_time.tv_sec - start_time.tv_sec) << " secs\n";
    
    log(LOG_INFO) << side_name << "killed\n";
	
	return 0;
}
