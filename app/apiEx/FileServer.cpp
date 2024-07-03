#include "FileServer.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

FileServer::FileServer(bool is_remote) : fileSent(false) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(0); // Let the OS choose the port

    if (is_remote) {
        serverAddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    updateServerInfo();

    std::cout << "Server listening on IP " << ip << " and port " << port << std::endl;
}

FileServer::~FileServer() {
    if (serverThread.joinable()) {
        serverThread.join();
    }
    close(serverSocket);
}

void FileServer::updateServerInfo() {
    socklen_t len = sizeof(serverAddr);
    if (getsockname(serverSocket, (struct sockaddr*)&serverAddr, &len) == -1) {
        std::cerr << "getsockname failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    port = ntohs(serverAddr.sin_port);
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddr.sin_addr), ip_str, INET_ADDRSTRLEN);
    ip = std::string(ip_str);
}

int FileServer::getPort() const {
    return port;
}

std::string FileServer::getIP() const {
    return ip;
}

size_t FileServer::getFileSize(const std::string& filename) const {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 0;
    }
    return file.tellg();
}

void FileServer::startInThread(const std::string& filename) {
    serverThread = std::thread(&FileServer::serverTask, this, filename);
}

void FileServer::serverTask(const std::string& filename) {
    std::unique_lock<std::mutex> lock(mtx);

    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    struct timeval timeout;
    timeout.tv_sec = 3; // 10 seconds timeout
    timeout.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);

    int activity = select(serverSocket + 1, &readfds, NULL, NULL, &timeout);

    if (activity < 0) {
        std::cerr << "Select error" << std::endl;
        return;
    } else if (activity == 0) {
        std::cerr << "Timeout occurred! No client connected within 10 seconds." << std::endl;
        return;
    }


    mClientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (mClientSocket < 0) {
        std::cerr << "Accept failed" << std::endl;
        return;
    }

    std::cout << "Client connected" << std::endl;
    sendFile(mClientSocket, filename);

    // 파일 전송이 완료되었음을 알림
    lock.unlock();

}

void FileServer::sendFile(int clientSocket, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    size_t fileSize = getFileSize(filename);
    size_t totalSent = 0;

    char buffer[BUFFER_SIZE];

    while (totalSent < fileSize) {
        file.read(buffer, BUFFER_SIZE);
        std::streamsize bytesRead = file.gcount();

        if (bytesRead == 0) {
            break; // 더 이상 읽을 데이터가 없음
        }

        size_t sent = 0;
        while (sent < bytesRead) {
            ssize_t bytesSent = send(clientSocket, buffer + sent, bytesRead - sent, 0);
            if (bytesSent < 0) {
                std::cerr << "데이터 전송 실패" << std::endl;
                close(clientSocket);
                return;
            }
            sent += bytesSent;
        }
        totalSent += bytesRead;
    }

    file.close();
}

void FileServer::receive(const std::string &filename)
{
    std::unique_lock<std::mutex> lock(mtx);
    std::cout << "Test" << std::endl;

    std::ofstream receivedFile(filename, std::ios::binary);
    if (!receivedFile.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    int bytesRead;
    char buffer[BUFFER_SIZE];
    while ((bytesRead = recv(mClientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        receivedFile.write(buffer, bytesRead);
    }
    std::cout << "receving done" << std::endl;

    receivedFile.close();
    close(mClientSocket);
}