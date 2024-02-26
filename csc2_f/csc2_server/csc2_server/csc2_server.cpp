#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <filesystem>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const std::string BASE_DIR = "D:\\csc2_f\\csc2_server\\csc2_server\\storage";

void handleClient(SOCKET clientSocket) {
	char buffer[1024];
	std::string clientName;

	int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived <= 0) {
		std::cerr << "Error receiving data from client" << std::endl;
		closesocket(clientSocket);
		return;
	}

	clientName.assign(buffer, bytesReceived);
	std::string clientDir = BASE_DIR + clientName + "\\";
	std::filesystem::create_directories(clientDir);

	while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		std::string command(buffer, bytesReceived);

		if (command == "LIST") {
			std::string fileList = "Available files:\n";
			for (const auto& entry : std::filesystem::directory_iterator(clientDir)) {
				fileList += entry.path().filename().string() + "\n";
			}
			send(clientSocket, fileList.c_str(), fileList.size(), 0);
		}
		else if (command.substr(0, 3) == "PUT") {
			std::string filename = command.substr(4);
			std::ofstream outFile(clientDir + filename, std::ios::binary);
			if (!outFile.is_open()) {
				std::string errMsg = "Failed to create file on server.";
				send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
			}
			else {
				char fileBuffer[1024];
				while ((bytesReceived = recv(clientSocket, fileBuffer, sizeof(fileBuffer), 0)) > 0) {
					outFile.write(fileBuffer, bytesReceived);
				}
				outFile.close();
				std::string successMsg = "File uploaded successfully.";
				send(clientSocket, successMsg.c_str(), successMsg.size(), 0);
			}
		}
		else if (command.substr(0, 3) == "GET") {
			std::string filename = command.substr(4);
			std::ifstream inFile(clientDir + filename, std::ios::binary);
			if (!inFile.is_open()) {
				std::string errMsg = "File not found.";
				send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
			}
			else {
				char fileBuffer[1024];
				while (inFile.read(fileBuffer, sizeof(fileBuffer)) || inFile.gcount() > 0) {
					send(clientSocket, fileBuffer, inFile.gcount(), 0);
					memset(fileBuffer, 0, sizeof(fileBuffer));
				}
				inFile.close();
			}
		}
		else if (command.substr(0, 6) == "DELETE") {
			std::string filename = command.substr(7);
			if (std::filesystem::remove(clientDir + filename)) {
				std::string successMsg = "File deleted successfully.";
				send(clientSocket, successMsg.c_str(), successMsg.size(), 0);
			}
			else {
				std::string errMsg = "Failed to delete file.";
				send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
			}
		}
		else {
			std::string errMsg = "Invalid command.";
			send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
		}

		memset(buffer, 0, sizeof(buffer));
	}

	closesocket(clientSocket);
}

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed." << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(12345);

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed." << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed." << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Server is listening on port 12345" << std::endl;

	while (true) {
		SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept failed." << std::endl;
			continue;
		}

		std::thread clientThread(handleClient, clientSocket);
		clientThread.detach();
	}

	closesocket(serverSocket);
	WSACleanup();
	return 0;
}
