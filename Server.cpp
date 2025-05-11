#include "includes.h"

void Server()
{
    //defines sockets
    int rtp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int rtcp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    
    //RTCP
    //defines how the sockets will be used using class sockaddr_in 
    sockaddr_in rtcp_addr; 
    rtcp_addr.sin_family = AF_INET; //defines that it is ipv4
    rtcp_addr.sin_port = htons(RTCP_Port); //Host to Network
    rtcp_addr.sin_addr.s_addr = INADDR_ANY; //Accepts packets from any IP address

    //binding socket to the OS: if we dont bind then the Socket wont listen on any port
    bind(rtcp_socket, (sockaddr*)&rtcp_addr, sizeof(rtcp_addr));

    //RTP
    //defines how the sockets will be used using class sockaddr_in 
    sockaddr_in rtp_addr; 
    rtp_addr.sin_family = AF_INET; //defines that it is ipv4
    rtp_addr.sin_port = htons(RTP_Port); //Host to Network
    rtp_addr.sin_addr.s_addr = INADDR_ANY; //Accepts packets from any IP address
    bind(rtp_socket, (sockaddr*)&rtp_addr, sizeof(rtp_addr));

    while (true)
    {
        
    }
    

}


