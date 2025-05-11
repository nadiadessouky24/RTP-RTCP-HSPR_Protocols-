#include "includes.h"

void Client() {
    int rtp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int rtcp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int opt = 1;
    setsockopt(rtp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(rtcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in client_rtp_addr{}, client_rtcp_addr{}, server_rtp_addr{};
    client_rtp_addr.sin_family = AF_INET;
    client_rtp_addr.sin_port = htons(CLIENT_RTP_PORT);
    client_rtp_addr.sin_addr.s_addr = INADDR_ANY;

    client_rtcp_addr.sin_family = AF_INET;
    client_rtcp_addr.sin_port = htons(CLIENT_RTCP_PORT);
    client_rtcp_addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(rtp_socket, (sockaddr*)&client_rtp_addr, sizeof(client_rtp_addr)) < 0)
        perror("RTP bind failed");
    if (::bind(rtcp_socket, (sockaddr*)&client_rtcp_addr, sizeof(client_rtcp_addr)) < 0)
        perror("RTCP bind failed");

    std::cout << "[CLIENT] RTCP bound to port " << CLIENT_RTCP_PORT << std::endl;

    server_rtp_addr.sin_family = AF_INET;
    server_rtp_addr.sin_port = htons(SERVER_RTP_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_rtp_addr.sin_addr);

    std::cout << "[CLIENT] Sending 5 RTP packets...\n";
    for (int seq = 1; seq <= 5; ++seq) {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::string msg = "SEQ=" + std::to_string(seq) + ";TIME=" + std::to_string(now);
        sendto(rtp_socket, msg.c_str(), msg.size(), 0, (sockaddr*)&server_rtp_addr, sizeof(server_rtp_addr));
        std::cout << "[SENT] " << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "[CLIENT] Waiting for RTCP feedback...\n";

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(rtcp_socket, &fds);
    timeval timeout = {5, 0};

    int activity = select(rtcp_socket + 1, &fds, nullptr, nullptr, &timeout);
    if (activity > 0 && FD_ISSET(rtcp_socket, &fds)) {
        char buffer[buffer_size];
        sockaddr_in from;
        socklen_t len = sizeof(from);
        int bytes = recvfrom(rtcp_socket, buffer, sizeof(buffer), 0, (sockaddr*)&from, &len);
        buffer[bytes] = '\0';
        std::cout << "[RTCP RECEIVED] " << buffer << std::endl;
    } else {
        std::cout << "[CLIENT] No RTCP feedback received.\n";
    }

    close(rtp_socket);
    close(rtcp_socket);
}
