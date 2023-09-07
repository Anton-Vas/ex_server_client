#include <iostream>
#include <string.h> 
#include <thread>
#include <netinet/in.h>     ///> sockaddr_in
#include <netdb.h>          ///> gethostbyname()
#include <arpa/inet.h>      ///> inet_ntoa()
#include <sys/time.h>       ///> gettimeofday()
#include <unistd.h>

#include <Logger.hpp>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CLIENT                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void TCP_client_func(){
    ///> setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
        /* Style */
    const string side_name {"\033[1;33mCLIENT\033[0;0m :"};
    
        /* Log */
    LOG_INIT_COUT();
    log(LOG_DONE) << side_name << "is alive\n";

        /* Sys */
    const char* TXT_SAMPLE  = "AAAAAAAA";
    const int   port_id     = 12345;                                        ///> port id
    const char* server_ip   = "127.0.0.1";                                  ///> server IP
    const int   msg_max_len = 4096;
    char        msg[msg_max_len];                                           ///> communication buffer (send/receive)
    struct hostent* host = gethostbyname(server_ip); 
    sockaddr_in client_socket;   

        /* setup socket and connection tools */
    bzero((char*)&client_socket, sizeof(client_socket));                      ///> explicitly erase data in memory of 'server_socket' struct
    client_socket.sin_family = AF_INET; 
    client_socket.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    client_socket.sin_port = htons(port_id);
    
    const int client_sd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sd < 0){
        log(LOG_ERR) << side_name << "socket() wasn`t established!\n";
        exit(1);
    }
    
    log(LOG_INFO) << side_name << "try to connect...\n";
    
    int status = connect(client_sd, (struct sockaddr*) &client_socket, sizeof(client_socket));
    if(status < 0)
    {
        log(LOG_ERR) << side_name << "ocker wasn`t connecter!\n";
        exit(1);
    }
    log(LOG_DONE) << side_name << "connected to the server\n";

        /* print */
    int bytes_read {0}, bytes_written {0};
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    ///> loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    while(true){

        memset(&msg, 0, sizeof(msg));
        strcpy(msg, TXT_SAMPLE);

        this_thread::sleep_for(1000ms);
        
        bytes_written += send(client_sd, (char*)&msg, strlen(msg), 0);
        log(LOG_DONE) << side_name << "send >> " << msg << "\n";

        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytes_read += recv(client_sd, (char*)&msg, sizeof(msg), 0);

        log(LOG_DONE) << side_name << "recv << " << msg << "\n";

        if(!strcmp(msg, "exit"))
        {
            log(LOG_DONE) << side_name << "server has quite the session\n";
            break;
        }
    }
    ///> end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|

    this_thread::sleep_for(3000ms);

    gettimeofday(&end_time, NULL);
    close(client_sd);

    log(LOG_INFO) << side_name << "******** Client Session ********\n";
    log(LOG_INFO) << side_name << "   | Bytes written: " << bytes_written << "\n";
    log(LOG_INFO) << side_name << "   | Bytes read:    " << bytes_read << "\n";
    log(LOG_INFO) << side_name << "   | Elapsed time:  " << (end_time.tv_sec - start_time.tv_sec) << " secs\n";

    log(LOG_INFO) << side_name << " killed\n";
}