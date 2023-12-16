#include <iostream>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // Create a socket
    int mallory_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (mallory_socket == -1) {
        std::cerr << "Error creating Mallory socket." << std::endl;
        return 1;
    }

    // Bind the socket to an address and port
    struct sockaddr_in mallory_address;
    mallory_address.sin_family = AF_INET;
    mallory_address.sin_port = htons(9090);  // Port 8080 for example
    mallory_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(mallory_socket, (struct sockaddr*)&mallory_address, sizeof(mallory_address)) == -1) {
        std::cerr << "Error binding Mallory socket." << std::endl;
        close(mallory_socket);
        return 1;
    }

    // Listen for incoming connections
    listen(mallory_socket, 1);

    // Accept a connection from Alice
    int alice_socket = accept(mallory_socket, nullptr, nullptr);
    if (alice_socket == -1) {
        std::cerr << "Error accepting connection from Alice." << std::endl;
        close(mallory_socket);
        return 1;
    }

    // Receive Alice's public key
    int alice_public_key;
    recv(alice_socket, &alice_public_key, sizeof(alice_public_key), 0);

    // Print Alice's public key intercepted by Mallory
    std::cout << "Mallory intercepted Alice's public key: " << alice_public_key << std::endl;

    // Manipulate the public key (subtracting 1 for example)
    int manipulated_alice_public_key = alice_public_key - 1;

    // Send the manipulated public key to Bob
    send(alice_socket, &manipulated_alice_public_key, sizeof(manipulated_alice_public_key), 0);

    // Close the sockets
    close(alice_socket);
    close(mallory_socket);

    return 0;
}
