#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct LocationData {
    time_t currentTime;
    int latitude;  
    int longitude;
    int altitude;
} typedef LocationData;


char* locationToMessage(LocationData location) {
    char* message = (char*)malloc(100 * sizeof(char));
    if (message == NULL) {
        perror("Bellek ayırma başarısız");
        exit(EXIT_FAILURE);
    }
    snprintf(message, 100, "Time: %ld, Latitude: %d, Longitude: %d, Altitude: %d",
             location.currentTime, location.latitude, location.longitude, location.altitude);
    return message;
}


char* encrypt(const char* message, int shift) {
    int len = strlen(message);
    char* encryptMessage = (char*)malloc((len + 1) * sizeof(char));
    if (encryptMessage == NULL) {
        perror("Bellek ayırma başarısız");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len; i++) {
        char ch = message[i];
        if (ch >= 'a' && ch <= 'z') {
            encryptMessage[i] = ((ch - 'a' + shift) % 26) + 'a';
        } else if (ch >= 'A' && ch <= 'Z') {
            encryptMessage[i] = ((ch - 'A' + shift) % 26) + 'A';
        } else {
            encryptMessage[i] = ch;
        }
    }
    encryptMessage[len] = '\0';
    return encryptMessage;
}


LocationData getLocation() {
    time_t currentTime = time(NULL);
    int latitude = rand() % 180 - 90;
    int longitude = rand() % 360 - 180;
    int altitude = rand() % 5000;

    LocationData location = {currentTime, latitude, longitude, altitude};
    return location;
}


void sendMessage(const char* encryptedMessage, const char* serverIP, int serverPort) {
    int sockfd;
    struct sockaddr_in serverAddr;

    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket oluşturma başarısız");
        exit(EXIT_FAILURE);
    }

    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    
    int n = sendto(sockfd, encryptedMessage, strlen(encryptedMessage), 0, 
                   (const struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (n == -1) {
        perror("Mesaj gönderme başarısız");
    } else {
        printf("Şifreli mesaj gönderildi: %s\n", encryptedMessage);
    }

    close(sockfd);
}

int main() {
    srand(time(NULL));
    LocationData location = getLocation();

    char* message = locationToMessage(location);
    printf("Mesaj: %s\n", message);

    int shift = 6;
    char* encryptedMessage = encrypt(message, shift);
    printf("Şifreli Mesaj: %s\n", encryptedMessage);

    const char* serverIP = "127.0.0.1";
    int serverPort = 7000;

    sendMessage(encryptedMessage, serverIP, serverPort);

    free(message);
    free(encryptedMessage);

    return 0;
}
