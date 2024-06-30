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
    std::lock_guard<std::mutex> lock(mtx);

    int clientSocket;
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


    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket < 0) {
        std::cerr << "Accept failed" << std::endl;
        return;
    }

    std::cout << "Client connected" << std::endl;
    sendFile(clientSocket, filename);
    close(clientSocket);

    // 파일 전송이 완료되었음을 알림
    fileSent = true;
}

void FileServer::sendFile(int clientSocket, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    while (file.read(buffer, BUFFER_SIZE)) {
        send(clientSocket, buffer, BUFFER_SIZE, 0);
    }

    if (file.gcount() > 0) {
        send(clientSocket, buffer, file.gcount(), 0);
    }

    file.close();
}
