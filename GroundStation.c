#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

const char* receiveMessage(int port) {

    static char buffer[1024];
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket oluşturma başarisiz");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  
    serverAddr.sin_port = htons(port);  

    
    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind başarısız");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /
    int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, 
                     (struct sockaddr *) &clientAddr, &addrLen);
    if (n < 0) {
        perror("Alma başarısız");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';  

    close(sockfd);  

    return buffer;  
}


const char* decrypt(const char* encryptedMessage) {
    int shift = 6;  
    int len = strlen(encryptedMessage);
    char* decryptMessage = (char*)malloc((len + 1) * sizeof(char));
    if (decryptMessage == NULL) {
        perror("Bellek ayırma başarısız");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len; i++) {
        char ch = encryptedMessage[i];
        if (ch >= 'a' && ch <= 'z') {
            decryptMessage[i] = ((ch - 'a' - shift + 26) % 26 + 'a');  
        } else if (ch >= 'A' && ch <= 'Z') {
            decryptMessage[i] = ((ch - 'A' - shift + 26) % 26 + 'A');  
        } else {
            decryptMessage[i] = ch;  
        }
    }
    decryptMessage[len] = '\0';  
    return decryptMessage;  
}


void displayMessage(const char* message) {
    printf("Çözülmüş şifre: %s\n", message);
}

int main() {
    int serverPort = 7000;  
    const char* encryptedMessage = receiveMessage(serverPort);  
    printf("Alınan şifreli mesaj: %s\n", encryptedMessage);  

    const char* decryptedMessage = decrypt(encryptedMessage);  
    displayMessage(decryptedMessage);  

    free((void*)decryptedMessage);  // Çözülmüş mesaj için belleği serbest bırak

    return 0;
}
