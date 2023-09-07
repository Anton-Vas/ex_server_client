#include <iostream>
#include <thread>

#include <Logger.hpp>

#include <TCP_client.cpp>
#include <TCP_server.cpp>
#include <UDP_client.cpp>
#include <UDP_server.cpp>
#include <TCP_chat_server.cpp>
#include <TCP_chat_clients.cpp>

using namespace std;
using namespace cpp_up;

///> DESCRIPTION:
/*
*   TASK:
*       to practice TCP/IP and UDP connections
*
*   SOURCE:
*       - TCP code              https://simpledevcode.wordpress.com/2016/06/16/client-server-chat-in-c-using-sockets/
*       - TCP add code          https://lenngro.github.io/how-to/2021/01/05/Simple-TCPIP-Server-Cpp/
*       - UDP code              https://www.geeksforgeeks.org/udp-server-client-implementation-c/
*       - TCP Multi clients     https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
*   
*   DEBUG :
*       $ nload -t 200 -i 1 -o 1 lo
*
*/

//////////////////////////
///> DESCRIPTION:    Cli <--> Ser                   (10 times until shutdown)
// #define TCP
///> DESCRIPTION:    Cli <--> Ser                   (10 times until shutdown)
// #define UDP
///> DESCRIPTION:    Cli_1 <--> Ser <--> Cli_2      (10 times until shutdown)
#define TCP_CHAT
//////////////////////////

int main(int, char**){
    
    LOG_INIT_COUT();
    log.set_log_level(LOG_DEBUG);
    log.set_log_style_status(LOG_STYLE_ON);

#if defined(TCP)
    cout << "\n################################### TCP/IP EXAMPLE ################################\n\n";

    thread* tcp_server = nullptr;
    thread* tcp_client = nullptr;

    tcp_server = new thread(TCP_server_func);
    this_thread::sleep_for(1000ms);
    tcp_client = new thread(TCP_client_func);
    
    if(tcp_server->joinable() && tcp_server != nullptr){
        tcp_server->join();
        log(LOG_DONE) << "server thread is joined\n";
    }
    if(tcp_client->joinable() && tcp_client != nullptr){
        tcp_client->join();
        log(LOG_DONE) << "client thread is joined\n";
    }

    tcp_client = nullptr;
    tcp_server = nullptr;
    delete tcp_client, tcp_server;

    this_thread::sleep_for(1000ms);
#endif
#if defined(UDP)
    cout << "\n################################### UDP EXAMPLE ###################################\n\n";

    thread* udp_server = nullptr;
    thread* udp_client = nullptr;

    udp_server = new thread(UDP_server_func);
    this_thread::sleep_for(500ms);
    udp_client = new thread(UDP_client_func);
    
    if(udp_server->joinable() && udp_server != nullptr){
        udp_server->join();
        log(LOG_DONE) << "server thread is joined\n";
    }
    if(udp_client->joinable() && udp_client != nullptr){
        udp_client->join();
        log(LOG_DONE) << "client thread is joined\n";
    }

    udp_client = nullptr;
    udp_server = nullptr;
    delete udp_client, udp_server;
#endif
#if defined(TCP_CHAT)
    cout << "\n################################### UDP CHAT ######################################\n\n";

    thread* tcp_server = nullptr;
    thread* tcp_client_one = nullptr;
    thread* tcp_client_two = nullptr;

    tcp_server = new thread(TCP_chat_server_func);
    this_thread::sleep_for(1000ms);
    tcp_client_one = new thread(TCP_chat_client_one_func);
    tcp_client_two = new thread(TCP_chat_client_two_func);

    if(tcp_server->joinable() && tcp_server != nullptr){
        tcp_server->join();
        log(LOG_WARN) << "SER thread is joined";
    }
    if(tcp_client_one->joinable() && tcp_client_one != nullptr){
        tcp_client_one->join();
        log(LOG_WARN) << "CLI 1 thread is joined";
    }
    if(tcp_client_two->joinable() && tcp_client_two != nullptr){
        tcp_client_two->join();
        log(LOG_WARN) << "CLI 2 thread is joined";
    }

    tcp_client_one = nullptr;
    tcp_client_two = nullptr;
    tcp_server = nullptr;
    delete tcp_client_one, tcp_client_two, tcp_server;

#endif
    cout << "\n################################### END ###########################################\n\n";
}
