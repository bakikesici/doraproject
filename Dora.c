#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct LocationData {
    time_t currentTime;
    int latitude;   // Latitude integer olarak
    int longitude;  // Longitude integer olarak
    int altitude;
} typedef LocationData;

// Konumu mesaj formatına çeviren fonksiyon
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

// Mesajı şifreleyen fonksiyon (Sezar şifreleme yöntemi)
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
            encryptMessage[i] = ((ch - 'a' + shift) % 26) + 'a';  // Küçük harfler için
        } else if (ch >= 'A' && ch <= 'Z') {
            encryptMessage[i] = ((ch - 'A' + shift) % 26) + 'A';  // Büyük harfler için
        } else {
            encryptMessage[i] = ch;  // Diğer karakterler için
        }
    }
    encryptMessage[len] = '\0';  // Sonuna null karakter ekle
    return encryptMessage;
}

// Konum bilgisi üreten fonksiyon
LocationData getLocation() {
    time_t currentTime = time(NULL);
    int latitude = rand() % 180 - 90;  // Latitude değeri (-90, 90) arası
    int longitude = rand() % 360 - 180;  // Longitude değeri (-180, 180) arası
    int altitude = rand() % 5000;  // Rastgele bir altitude değeri

    LocationData location = {currentTime, latitude, longitude, altitude};
    return location;
}

// Şifrelenmiş mesajı sunucuya gönderen fonksiyon
void sendMessage(const char* encryptedMessage, const char* serverIP, int serverPort) {
    int sockfd;
    struct sockaddr_in serverAddr;

    // UDP soketi oluştur
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket oluşturma başarısız");
        exit(EXIT_FAILURE);
    }

    // Sunucu adresini yapılandır
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    // Mesajı gönder
    int n = sendto(sockfd, encryptedMessage, strlen(encryptedMessage), 0, 
                   (const struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (n == -1) {
        perror("Mesaj gönderme başarısız");
    } else {
        printf("Şifreli mesaj gönderildi: %s\n", encryptedMessage);
    }

    close(sockfd);  // Soketi kapat
}

int main() {
    srand(time(NULL));  // Rastgelelik için zaman değeri
    LocationData location = getLocation();  // Konumu al

    char* message = locationToMessage(location);  // Mesajı oluştur
    printf("Mesaj: %s\n", message);

    int shift = 6;  // Şifreleme kaydırma miktarı
    char* encryptedMessage = encrypt(message, shift);  // Mesajı şifrele
    printf("Şifreli Mesaj: %s\n", encryptedMessage);

    const char* serverIP = "127.0.0.1";  // Groundstation IP'si (localhost)
    int serverPort = 7000;  // Groundstation portu

    sendMessage(encryptedMessage, serverIP, serverPort);  // Mesajı gönder

    free(message);  // Belleği temizle
    free(encryptedMessage);  // Belleği temizle

    return 0;
}
