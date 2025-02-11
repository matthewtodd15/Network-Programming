// client.cpp
// To Compile: g++ client.cpp -o client
// To Run: ./client

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Step 1: Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert IPv4 and connect
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    cout << "Connected to WebSocket server.\n";

    // Step 2: Send WebSocket handshake request
    string handshake =
        "GET / HTTP/1.1\r\n"
        "Host: " + string(SERVER_IP) + ":" + to_string(SERVER_PORT) + "\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dummy_key\r\n"
        "Sec-WebSocket-Version: 13\r\n\r\n";

    send(sock, handshake.c_str(), handshake.length(), 0);
    read(sock, buffer, BUFFER_SIZE);
    cout << "Handshake response:\n" << buffer << endl;

    // Step 3: Send WebSocket message
    while (true) {
        string message;
        cout << "Enter message (or type 'exit' to quit): ";
        getline(cin, message);

        if (message == "exit") break;

        // Send a simple WebSocket frame
        string frame = "\x81" + string(1, static_cast<char>(message.size())) + message;
        send(sock, frame.c_str(), frame.size(), 0);

        // Receive echo response
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        cout << "Server response: " << buffer + 2 << endl; // Skip WebSocket frame headers
    }

    // Step 4: Close socket
    close(sock);
    return 0;
}
