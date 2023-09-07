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
//  CLIENT  1                                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void TCP_chat_client_one_func(){
    ///> init ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    /*
    *   Style
    */
    const string        name {"\033[1;33mCLI 1\033[0;0m :"};
    
    /*
    *   Log
    */
    LOG_INIT_COUT();
    log(LOG_DONE) << name << "is alive";

    /*
    *   System
    */
    const char*         TXT_SAMPLE          {"_text_for_cli_2_"};
    const char*         server_ip           {"127.0.0.1"};
    const int           port_id             {11111};
    const int           msg_max_len         {4096};
    int                 bytes_read          {0};
    int                 bytes_written       {0};
    int                 bytes_per_call      {0};
    int                 client_fd           {0};
    int                 status              {0};
    int                 count_msg           {0};
    char                msg[msg_max_len];
    struct timeval      start_time;
    struct timeval      end_time;
    struct hostent*     host                = gethostbyname(server_ip); 
    sockaddr_in         client_addr;

    log(LOG_INFO) << name << "Port ID   " << port_id << "";
    log(LOG_INFO) << name << "Server ID " << server_ip << "";

    ///> setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if( client_fd <= 0) { log(LOG_ERR) << name << "socket(), errno: " << strerror(errno) << ""; exit(1); }
    else                { log(LOG_DONE) << name << "socket() is completed"; }

    bzero((char*)&client_addr, sizeof(client_addr));
    client_addr.sin_family        = AF_INET; 
    client_addr.sin_addr.s_addr   = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    client_addr.sin_port          = htons(port_id);
    
    status = connect(client_fd, (struct sockaddr*) &client_addr, sizeof(client_addr));
    if(status < 0)  { log(LOG_ERR) << name << "connect(), errno: " << strerror(errno) << ""; exit(1); }
    else            { log(LOG_DONE) << name << "connect() is completed"; }

    ///> loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    gettimeofday(&start_time, NULL);
    while(true){

        this_thread::sleep_for(1000ms);

            /* Decide which mag to send */
        memset(&msg, 0, sizeof(msg));
        if (count_msg >= 10){
            strcpy(msg, "exit");
        }
        else{
            strcpy(msg, TXT_SAMPLE);
        }
        
            /* Send */
        bytes_per_call = send(client_fd, (char*)&msg, strlen(msg), 0);
        if (bytes_per_call > 0){
            bytes_written += bytes_per_call;
            log(LOG_DONE) << name << "send >> " << msg << "";
        }
        else if (bytes_per_call < 0){
            log(LOG_WARN) << name << "send(), errno: " << strerror(errno) << "";
        }
    
        memset(&msg, 0, sizeof(msg));

            /* Receive */
        bytes_per_call = recv(client_fd, (char*)&msg, sizeof(msg), 0);
        if (bytes_per_call > 0){
            bytes_read += bytes_per_call;
            log(LOG_DONE) << name << "recv << " << msg << "";
        }
        else if (bytes_per_call < 0){
            log(LOG_WARN) << name << "recv(), errno: " << strerror(errno) << "";
        }

            /* Count received msgs from CLI 2 */
        if (!strcmp(msg, "_text_for_cli_1_")){
            count_msg++;
        }

            /* Exit */
        if (!strcmp(msg, "exit"))
        {
            log(LOG_INFO) << name << "Session END";
            break;
        }
    }
    gettimeofday(&end_time, NULL);
    ///> end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|

    this_thread::sleep_for(3000ms);

    close(client_fd);

    log(LOG_INFO) << name << "******** CLI 1 Session ************************";
    log(LOG_INFO) << name << "   | Bytes written: " << bytes_written << "";
    log(LOG_INFO) << name << "   | Bytes read:    " << bytes_read << "";
    log(LOG_INFO) << name << "   | Elapsed time:  " << (end_time.tv_sec - start_time.tv_sec) << " secs";
    log(LOG_INFO) << name << "***********************************************";

    log(LOG_INFO) << name << " killed";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CLIENT  2                                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void TCP_chat_client_two_func(){
    ///> init ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    /*
    *   Style
    */
    const string        name {"\033[1;32mCLI 2\033[0;0m :"};
    
    /*
    *   Log
    */
    LOG_INIT_COUT();
    log(LOG_DONE) << name << "is alive";

    /*
    *   System
    */
    const char*         TXT_SAMPLE          {"_text_for_cli_1_"};
    const char*         server_ip           {"127.0.0.1"};
    const int           port_id             {11111};
    const int           msg_max_len         {4096};
    int                 bytes_read          {0};
    int                 bytes_written       {0};
    int                 bytes_per_call      {0};
    int                 client_fd           {0};
    int                 status              {0};
    char                msg[msg_max_len];
    struct timeval      start_time;
    struct timeval      end_time;
    struct hostent*     host                = gethostbyname(server_ip); 
    sockaddr_in         client_addr;

    log(LOG_INFO) << name << "Port ID   " << port_id;
    log(LOG_INFO) << name << "Server ID " << server_ip;

    ///> setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if( client_fd <= 0) { log(LOG_ERR) << name << "socket(), errno: " << strerror(errno); exit(1); }
    else                { log(LOG_DONE) << name << "socket() is completed"; }

    bzero((char*)&client_addr, sizeof(client_addr));
    client_addr.sin_family        = AF_INET; 
    client_addr.sin_addr.s_addr   = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    client_addr.sin_port          = htons(port_id);
    
    status = connect(client_fd, (struct sockaddr*) &client_addr, sizeof(client_addr));
    if(status < 0)  { log(LOG_ERR) << name << "connect(), errno: " << strerror(errno); exit(1); }
    else            { log(LOG_DONE) << name << "connect() is completed"; }

    ///> loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    gettimeofday(&start_time, NULL);
    while(true){

        this_thread::sleep_for(1000ms);

            /* Decide which mag to send */
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, TXT_SAMPLE);
        
            /* Send */
        bytes_per_call = send(client_fd, (char*)&msg, strlen(msg), 0);
        if (bytes_per_call > 0){
            bytes_written += bytes_per_call;
            log(LOG_DONE) << name << "send >> " << msg;
        }
        else if (bytes_per_call < 0){
            log(LOG_WARN) << name << "send(), errno: " << strerror(errno);
        }
    
        memset(&msg, 0, sizeof(msg));

            /* Receive */
        bytes_per_call = recv(client_fd, (char*)&msg, sizeof(msg), 0);
        if (bytes_per_call > 0){
            bytes_read += bytes_per_call;
            log(LOG_DONE) << name << "recv << " << msg;
        }
        else if (bytes_per_call < 0){
            log(LOG_WARN) << name << "recv(), errno: " << strerror(errno);
        }

            /* Exit */
        if (!strcmp(msg, "exit"))
        {
            log(LOG_INFO) << name << "Session END";
            break;
        }
    }
    gettimeofday(&end_time, NULL);
    ///> end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|

    this_thread::sleep_for(3000ms);

    close(client_fd);

    log(LOG_INFO) << name << "******** CLI 1 Session ************************";
    log(LOG_INFO) << name << "   | Bytes written: " << bytes_written;
    log(LOG_INFO) << name << "   | Bytes read:    " << bytes_read;
    log(LOG_INFO) << name << "   | Elapsed time:  " << (end_time.tv_sec - start_time.tv_sec) << " secs";
    log(LOG_INFO) << name << "***********************************************";

    log(LOG_INFO) << name << " killed";
}