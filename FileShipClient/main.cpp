#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>

#define print(msg) std::cout << msg << std::endl

int connectToServer(const char *serverAddress, int serverPort) {
    int sock;
    struct sockaddr_in header;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        print("An error caused during initiating socket");
        return -1;
    }
    bzero(&header, sizeof(header));
    header.sin_family = AF_INET;
    header.sin_addr.s_addr = inet_addr(serverAddress);
    header.sin_port = htons(serverPort);
    if (connect(sock, (struct sockaddr *)&header, sizeof(header)) < 0) {
        print("Couldn't establish connection");
        close(sock);
        return -1;
    }
    print("Established connection");
    return sock;
}

char *readFile(std::ifstream &file, unsigned long &sizeInBytes) {
    file.seekg(0, std::ios::end);
    std::ifstream::pos_type size = file.tellg();
    sizeInBytes = (unsigned long)size;
    file.seekg(0, std::ios::beg);
    char *memBlock = new char[size];
    file.read(memBlock, size);
    std::cout << "Reading " << size << " Bytes" << std::endl;
    return memBlock;
}

void sendData(int sock, const char *filePath, int maxDataSize) {
    std::ifstream file;
    std::string fileName = filePath;
    char *dataStartAddr;
    unsigned long dataSize;
    unsigned int bytesSend = 0;
    int ret;
    file.open(filePath);
    if (!file.is_open()) {
        return;
    }
    fileName = fileName.substr(fileName.find_last_of("/") + 1);
    send(sock, fileName.c_str(), maxDataSize, 0);
    dataStartAddr = readFile(file, dataSize);
    while (bytesSend < dataSize) {
        ret = send(sock, dataStartAddr, dataSize - bytesSend, 0);
        if (ret <= 0) {
            return;
        }
        bytesSend += ret;
    }
    file.close();
}

int main(int argc, char *argv[]) {
    int sock;
    int port;
    std::ifstream file;
    std::string fileData;
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " [server addr] [server port] [file to transfer]" << std::endl;
        return 1;
    }
    sscanf(argv[2], "%d", &port);
    sock = connectToServer(argv[1], port);
    sendData(sock, argv[3], 1000);
    return 0;
}
