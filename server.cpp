// server.cpp
// To Compile: g++ server.cpp -o server
// To Run: ./server
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Step 1: Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Step 3: Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    cout << "WebSocket Server listening on port " << PORT << "...\n";

    while (true) {
        // Step 4: Accept incoming connections
        client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Step 5: Read handshake request
        read(client_fd, buffer, BUFFER_SIZE);
        string request(buffer);

        // Step 6: Send WebSocket handshake response (simplified)
        string response = 
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: dummy_key\r\n\r\n";

        send(client_fd, response.c_str(), response.length(), 0);
        cout << "Handshake complete with client.\n";

        // Step 7: Handle WebSocket messages
        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = read(client_fd, buffer, BUFFER_SIZE);
            if (valread <= 0) break;

            string message(buffer + 2, valread - 2); // Skip WebSocket frame headers
            cout << "Received: " << message << endl;

            // Send an echo response
            string reply = "Echo: " + message;
            vector<unsigned char> frame = {0x81, static_cast<unsigned char>(reply.size())};
            frame.insert(frame.end(), reply.begin(), reply.end());

            send(client_fd, reinterpret_cast<char*>(frame.data()), frame.size(), 0);
        }

        // Step 8: Close the client connection
        close(client_fd);
    }

    // Close the server socket
    close(server_fd);
    return 0;
}

