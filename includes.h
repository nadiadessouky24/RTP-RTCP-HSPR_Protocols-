#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstring>
#include <cctype>
#include <cmath>
#include <vector>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_RTP_PORT 5000
#define SERVER_RTCP_PORT 5001
#define CLIENT_RTP_PORT 6000
#define CLIENT_RTCP_PORT 6001
#define buffer_size 1024

void Client();
void Server();
