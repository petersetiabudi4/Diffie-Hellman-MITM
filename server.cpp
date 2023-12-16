#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

// Function to calculate modular exponentiation (a^b mod m)
int mod_exp(int base, int exponent, int mod) {
    int result = 1;
    base = base % mod;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exponent = exponent / 2;
    }
    return result;
}

// Function to perform Diffie-Hellman key exchange
int diffie_hellman(int base, int prime) {
    int private_key, public_key;
    
    // Bob generates his private key
    private_key = rand() % (prime - 2) + 1;

    // Calculate public key
    public_key = mod_exp(base, private_key, prime);

    return public_key;
}

int main() {
    // Shared prime and base
    int prime = 23;   // A small prime for demonstration purposes
    int base = 5;     // A primitive root modulo prime

    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating server socket." << std::endl;
        return 1;
    }

    // Bind the socket to an address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9090);  // Port 8080 for example
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error binding server socket." << std::endl;
        close(server_socket);
        return 1;
    }

    // Listen for incoming connections
    listen(server_socket, 1);

    // Accept a connection
    int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == -1) {
        std::cerr << "Error accepting connection." << std::endl;
        close(server_socket);
        return 1;
    }

    // Receive Alice's public key
    int alice_public_key;
    recv(client_socket, &alice_public_key, sizeof(alice_public_key), 0);

    // Print Alice's public key (for debugging)
    std::cout << "Bob received Alice's public key: " << alice_public_key << std::endl;

    // Bob generates his public key
    int bob_public_key = diffie_hellman(base, prime);

    // Send Bob's public key to Alice
    send(client_socket, &bob_public_key, sizeof(bob_public_key), 0);

     // Bob computes the shared secret
    int bob_shared_secret = mod_exp(alice_public_key, bob_public_key, prime);
    std::cout << "Bob's shared secret: " << bob_shared_secret << std::endl;


    // Close the sockets
    close(client_socket);
    close(server_socket);

    return 0;
}