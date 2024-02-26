#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

const std::string STORAGE_DIR = "C:\\Users\\Admin\\source\\repos\\csc2_f\\csc2_client\\csc2_client\\client_storage\\";

void sendCommand(SOCKET sock, const std::string& command) {
	send(sock, command.c_str(), command.length(), 0);
}

void sendFile(SOCKET sock, const std::string& filename) {
	std::string filePath = STORAGE_DIR + filename; 
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filePath << std::endl;
		return;
	}

	char buffer[1024];
	while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
		send(sock, buffer, file.gcount(), 0);
		memset(buffer, 0, sizeof(buffer));
	}

	file.close();
}

void receiveFile(SOCKET sock, const std::string& filename) {
	std::string filePath = STORAGE_DIR + "received_" + filename; 
	std::ofstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to create file: " << filePath << std::endl;
		return;
	}

	char buffer[1024];
	int bytesRead;
	while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
		file.write(buffer, bytesRead);
	}

	file.close();
}

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(12345);
	InetPton(AF_INET, TEXT("127.0.0.1"), &serverAddr.sin_addr);

	connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

	std::string clientName = "Client_2";
	sendCommand(sock, clientName); 

	std::string command;
	while (true) {
		std::cout << "Enter command (LIST, PUT <filename>, GET <filename>, DELETE <filename>, or EXIT): ";
		std::getline(std::cin, command);

		if (command == "EXIT") {
			break;
		}
		else if (command.substr(0, 3) == "PUT") {
			std::string filename = command.substr(4);
			sendCommand(sock, command); 
			sendFile(sock, filename); 
		}
		else if (command.substr(0, 3) == "GET") {
			sendCommand(sock, command); 
			std::string filename = command.substr(4);
			receiveFile(sock, filename); 
		}
		else {
			sendCommand(sock, command); 
		}

		char buffer[1024] = { 0 };
		int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
		if (bytesReceived > 0) {
			std::cout << "Server response:\n" << std::string(buffer, bytesReceived) << std::endl;
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}
