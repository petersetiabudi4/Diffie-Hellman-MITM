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
    
    // Alice generates her private key
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
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating client socket." << std::endl;
        return 1;
    }

    // Connect to Mallory (pretending to be Bob)
    struct sockaddr_in mallory_address;
    mallory_address.sin_family = AF_INET;
    mallory_address.sin_port = htons(9090);  // Port 8080 for example
    mallory_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Assuming Mallory is on localhost
    if (connect(client_socket, (struct sockaddr*)&mallory_address, sizeof(mallory_address)) == -1) {
        std::cerr << "Error connecting to Mallory." << std::endl;
        close(client_socket);
        return 1;
    }

    // Alice generates her public key
    int alice_public_key = diffie_hellman(base, prime);

    // Send Alice's public key to Mallory (pretending to be Bob)
    send(client_socket, &alice_public_key, sizeof(alice_public_key), 0);

    // Receive manipulated public key from Mallory
    int manipulated_bob_public_key;
    recv(client_socket, &manipulated_bob_public_key, sizeof(manipulated_bob_public_key), 0);

    // Alice computes the shared secret with the manipulated public key
    int alice_shared_secret = mod_exp(manipulated_bob_public_key, alice_public_key, prime);
    std::cout << "Alice's shared secret: " << alice_shared_secret << std::endl;

    // Close the socket
    close(client_socket);

    return 0;
}
