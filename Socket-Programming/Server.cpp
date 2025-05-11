#include "includes.h"

int extract_sequence_num(char* msg) {
    char* p = strstr(msg, "SEQ=");
    return p ? atoi(p + 4) : -1;
}

long long extract_send_time(char* msg) {
    char* p = strstr(msg, "TIME=");
    return p ? atoll(p + 5) : 0;
}

void Server() {
    int rtp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int rtcp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int opt = 1;
    setsockopt(rtp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(rtcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in rtp_addr{}, rtcp_addr{};
    rtp_addr.sin_family = AF_INET;
    rtp_addr.sin_port = htons(SERVER_RTP_PORT);
    rtp_addr.sin_addr.s_addr = INADDR_ANY;

    rtcp_addr.sin_family = AF_INET;
    rtcp_addr.sin_port = htons(SERVER_RTCP_PORT);
    rtcp_addr.sin_addr.s_addr = INADDR_ANY;

    ::bind(rtp_socket, (sockaddr*)&rtp_addr, sizeof(rtp_addr));
    ::bind(rtcp_socket, (sockaddr*)&rtcp_addr, sizeof(rtcp_addr));

    std::cout << "[SERVER] Listening on RTP " << SERVER_RTP_PORT << ", RTCP " << SERVER_RTCP_PORT << std::endl;

    int last_seq = -1;
    int total_expected = 0;
    int total_received = 0;
    float avg_jitter = 0.0f;
    std::chrono::steady_clock::time_point last_arrival;

    while (true) {
        char buffer[buffer_size];
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);

        int bytes = recvfrom(rtp_socket, buffer, sizeof(buffer), 0, (sockaddr*)&client_addr, &len);
        buffer[bytes] = '\0';

        int seq = extract_sequence_num(buffer);
        long long client_send_time = extract_send_time(buffer);

        auto now_system = std::chrono::system_clock::now();
        long long server_recv_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                                         now_system.time_since_epoch()).count();
        long long delay = server_recv_time - client_send_time;

        total_received++;
        if (last_seq != -1 && seq > last_seq) {
            total_expected += (seq - last_seq);
        } else if (last_seq == -1) {
            total_expected = 1;
        }

        // Calculate inter-arrival jitter
        auto now_steady = std::chrono::steady_clock::now();
        if (last_seq != -1) {
            long long inter_arrival = std::chrono::duration_cast<std::chrono::milliseconds>(now_steady - last_arrival).count();
            float d = std::abs(inter_arrival - avg_jitter);
            avg_jitter += (d - avg_jitter) / 16.0f;
        }
        last_arrival = now_steady;
        last_seq = seq;

        float packet_loss = (total_expected > 0) ? (100.0f * (total_expected - total_received) / total_expected) : 0.0f;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        std::cout << "[RECV] From " << ip << ":" << ntohs(client_addr.sin_port) << " | "
                  << buffer << " | Delay: " << delay << "ms | Loss: "
                  << packet_loss << "% | Jitter: " << avg_jitter << "ms" << std::endl;

        if (total_received % 5 == 0) {
            char feedback[128];
            snprintf(feedback, sizeof(feedback),
                     "RTCP: delay=%lldms, loss=%.1f%%, jitter=%.1fms, seq=%d",
                     delay, packet_loss, avg_jitter, seq);

            sockaddr_in rtcp_dest = client_addr;
            rtcp_dest.sin_port = htons(CLIENT_RTCP_PORT);

            std::cout << "[RTCP SENT TO] " << ip << ":" << CLIENT_RTCP_PORT << std::endl;
            int sent = sendto(rtcp_socket, feedback, strlen(feedback), 0,
                              (sockaddr*)&rtcp_dest, sizeof(rtcp_dest));
            if (sent < 0)
                perror("sendto RTCP failed");
            else
                std::cout << "[RTCP SENT] " << feedback << std::endl;
        }
    }
}
