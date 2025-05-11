#include "includes.h"

int main() 
{
    thread server_thread(Server);

    thread client_thread(Client);

    server_thread.join();
    client_thread.join();

    return 0;
}
