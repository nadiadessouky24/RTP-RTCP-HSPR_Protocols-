#pragma once 
#include <iostream>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include "Server.h"
#include "Client.h"
using namespace std;

#define RTP_Port 5000
#define RTCP_Port 5001
#define buffer 1024