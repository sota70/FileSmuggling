#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/ioctl.h>
#include <tuple>

#define SERVER_PORT 6667
#define print(msg) std::cout << msg << std::endl


int initiateSocket() {
    int sock;
    struct sockaddr_in header;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    bzero(&header, sizeof(header));
    header.sin_family = AF_INET;
    header.sin_addr.s_addr = INADDR_ANY;
    header.sin_port = htons(SERVER_PORT);
    if (bind(sock, (struct sockaddr *)&header, sizeof(header)) < 0) {
        return -1;
    }
    return sock;
}

std::tuple<char *, std::string> readData(int sock) {
    std::string finalBuff;
    int dataSize = 1024;
    char *fileName = (char *)malloc(sizeof(char) * dataSize);
    char *tempBuff;
    bzero(fileName, dataSize);
    recv(sock, fileName, dataSize, MSG_WAITALL);
    while (true) {
        tempBuff = (char *)malloc(sizeof(char) * dataSize);
        bzero(tempBuff, dataSize);
        if (recv(sock, tempBuff, dataSize, MSG_WAITALL) < 1) {
            break;
        }
        finalBuff.append(tempBuff);
        free(tempBuff);
    }
    return std::make_tuple(fileName, finalBuff);
}

void writeData(const char *fileName, std::string &data) {
    std::string dataDir = "data/";
    std::ofstream file;
    std::cout << "FILENAME: " << fileName << std::endl;
    dataDir.append(fileName);
    file.open(dataDir);
    if (!file.is_open()) {
        print("File isn't opened");
        return;
    }
    file << data.c_str();
}

int main() {
    int sock = initiateSocket();
    int clientSock;
    std::tuple<char *, std::string> recvData;
    if (sock < 0) {
        print("An error caused during initiating socket");
        return 1;
    }
    if (listen(sock, SOMAXCONN) < 0) {
        print("An error caused during trying to listen connections");
        close(sock);
        return 1;
    }
    print("Waiting for connections");
    while (true) {
        if ((clientSock = accept(sock, nullptr, nullptr)) < 0) {
            print("An error caused during accepting client connection");
            continue;
        }
        print("New connection established");
        recvData = readData(clientSock);
        print(std::get<1>(recvData).c_str());
        writeData(std::get<0>(recvData), std::get<1>(recvData));
        free(std::get<0>(recvData));
        close(clientSock);
    }
    close(sock);
    return 0;
}