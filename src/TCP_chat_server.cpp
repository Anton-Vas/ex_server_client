#include <iostream>
#include <string>
#include <thread>
#include <array>
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

static void TCP_chat_server_func(){

    ///> setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    /*
    *   Style
    */
    const string            name {"\033[1;36mSERVER\033[0;0m :"};

    /*
    *   Log
    */
    LOG_INIT_COUT();
    log(LOG_DONE) << name << "is alive\n";

    /*
    *   System
    */
    const int           port_id             {11111};
    const int           msg_max_len         {4096};
    const int           listen_connections  {3};
    array<int, 2>       client_addr         {{0, 0}};
    int                 opt                 {1};
    int                 sd;
    int                 activity;
    int                 new_fd;
    int                 server_fd ;
    int                 bytes_read          {0};
    int                 bytes_written       {0};
    int                 bytes_per_call      {0};
    char                msg[msg_max_len];
    struct timeval      start_time;
    struct timeval      end_time;
    struct sockaddr_in  server_addr;
    fd_set              read_fds;

    /*
    *   System flags
    */
    int                 setsockopt_status   {0};
    int                 listen_status       {0};
    int                 bind_status         {0};
    
    /*
    *   Server server socket
    */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd <= 0) { log(LOG_ERR) << name << "socket(), errno: " << strerror(errno) << "\n"; exit(1); }
    else                { log(LOG_DONE) << name << "socket() is completed"; }

    /*
    *   Set server socket to allow multi connections
    */
    setsockopt_status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    if (setsockopt_status < 0)  { log(LOG_ERR) << name << "setsockopt(), errno: " << strerror(errno); exit(1); }
    else                        { log(LOG_DONE) << name << "setsockopt() is completed"; }

    /*
    *   Set server socket with standard settings
    */
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_id);

    /*
    *   Bind
    */
    bind_status = bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (bind_status < 0) { log(LOG_ERR) << name << "bind(), errno: " << strerror(errno); exit(1); }
    else                 { log(LOG_DONE) << name << "bind() is completed"; }

    /*
    *   Listen for 
    */
    listen_status = listen(server_fd, listen_connections);
    if (listen_status < 0)  { log(LOG_ERR) << name << "listen(), errno: " << strerror(errno); exit(1); }
    else                    { log(LOG_DONE) << name << "listen() is completed"; }
    log(LOG_DONE) << name << "listen() is completed";


    ///> loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
    gettimeofday(&start_time, NULL);
    while(true){

        /* Clear the socket set */
        FD_ZERO(&read_fds);

        /* Add Server socket to set */
        FD_SET(server_fd, &read_fds);
        int max_sd = server_fd;

        /* Add clients sockets to set */
        for(int i = 0; i < client_addr.size(); i++){
            /* socket fd */
            sd = client_addr.at(i);
            /* if valid socket descriptor then add to read list */
            if (sd > 0){
                FD_SET(sd, &read_fds);
            }
            /* highest file descriptor number, need it for the select function */
            if (sd > max_sd){
                max_sd = sd;
            }
        }

        /* Wait for an activity on one of the sockets, timeout = NULL, so wait indefinitely */
        activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR){
            log(LOG_ERR) << name << "select(), errno: " << strerror(errno) << "\n";
        }

        /* Activity on the server socket -> incoming msg*/
        if (FD_ISSET(server_fd, &read_fds))
		{
            new_fd  = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&server_addr);
			if (new_fd < 0) { log(LOG_ERR) << name << "accept(), errno: " << strerror(errno); }
            else            { log(LOG_DONE) << name << "accept() is completed"; }
		
			/* Send new connection greeting message */
            bytes_per_call = send(new_fd, "HI from the SERVER", strlen("HI from the SERVER"), 0);	
            if (bytes_per_call > 0){
                bytes_written += bytes_per_call;
                log(LOG_DONE) << name << "send >> " << msg;
            }
            else if (bytes_per_call < 0){
                log(LOG_WARN) << name << "send(), errno: " << strerror(errno);
            }
				
			/* Add new socket to array of sockets */
			for (int i = 0; i < client_addr.size(); i++)
			{
				/* If position is empty */
				if( client_addr.at(i) == 0 ){
					client_addr.at(i) = new_fd;
					log(LOG_DONE) << name << "add a CLI socket\n";
					break;
				}
			}
		}

        /* Else some IO operation on other socket */
        for (int i = 0; i < client_addr.size(); i++)
		{
			sd = client_addr.at(i);
				
			if (FD_ISSET( sd , &read_fds))
			{
				/* Check if it was for closing , and also read the incoming message */
                bytes_per_call = recv(sd, (char*)&msg, sizeof(msg), 0);
                if (bytes_per_call > 0){
                    bytes_read += bytes_per_call;
                    log(LOG_DONE) << name << "recv << " << msg;
                }
                else if (bytes_per_call < 0){
                    log(LOG_WARN) << name << "recv(), errno: " << strerror(errno);
                }
                
                if (bytes_per_call == 0){
					//Somebody disconnected , get his details and print
                    int server_addr_len = sizeof(server_addr);
					getpeername(sd , (struct sockaddr*)&server_addr , (socklen_t*)&server_addr_len);
					log(LOG_WARN) << name << "host disconnected";

					/* Close the socket and mark as 0 in list for reuse */
					close( sd );
					client_addr.at(i) = 0;
				}
				/* Echo back the message that came in */
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					send(sd , msg , strlen(msg) , 0 );
				}
			}
		}
    }
    gettimeofday(&end_time, NULL);
    ///> end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|

    this_thread::sleep_for(1000ms);

    close(new_fd);
    close(server_fd);

    log(LOG_INFO) << name << "******** SER Session **************************";
    log(LOG_INFO) << name << "   | Bytes written: " << bytes_written;
    log(LOG_INFO) << name << "   | Bytes read:    " << bytes_read;
    log(LOG_INFO) << name << "   | Elapsed time:  " << (end_time.tv_sec - start_time.tv_sec) << " secs";
    log(LOG_INFO) << name << "***********************************************";
    
    log(LOG_INFO) << name << " killed";
}