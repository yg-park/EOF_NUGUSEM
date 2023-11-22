#include "pch.h"
#include "Server.h"

Server::Server() {
    image_flag = false;//image ���� flag

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // TCP socket ����
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);

    // ���� �ּ� ����
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // ���� ���ε�
    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // ���� ���� ���
    listen(serverSocket, 5);
    std::cout << "Server listening on port " << PORT << "..." << std::endl;
    
}

Server::~Server() {
    closesocket(serverSocket);
    WSACleanup();
}

void Server::run(CString &receieved_string) {
    
    // Ŭ���̾�Ʈ ���� ����
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    // ���� 1 accept �� ��� ������ ���� ������ΰ� ����.
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);


    // Receive data type header
    DataType dataType;
    int headerBytesRead = recv(clientSocket, reinterpret_cast<char*>(&dataType), sizeof(DataType), 0);


    if (headerBytesRead != sizeof(DataType)) {
        std::cerr << "Error reading data type header" << std::endl;
        closesocket(clientSocket);
    }

    // Process data based on type
    switch (dataType) {
    case IMAGE:
        receiveImage(clientSocket);
        //image_flag 1
        break;
    case STRING:
        receieved_string = receiveString(clientSocket);
        break;
    default:
        std::cerr << "Unknown data type received" << std::endl;
    }

    // ���� ����
    closesocket(clientSocket);
    
}

void Server::set_image_flag(bool image_flag)
{
    this->image_flag = image_flag;
}

bool Server::get_image_flag()
{
    return this->image_flag;
}

void Server::receiveImage(SOCKET clientSocket) {
    // �̹��� ���� ����
    std::ofstream receivedFile("received_image.png", std::ios::binary);

    // �̹��� ������ ���� �� ����
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        receivedFile.write(buffer, bytesRead);
        std::cout << "Received " << bytesRead << " bytes of image data from client" << std::endl;
    }

    receivedFile.close();
    set_image_flag(true);
    std::cout << "Image received and saved as received_image.png" << std::endl;
}

CString Server::receiveString(SOCKET clientSocket) {
    // ���ڿ� ����
    char stringBuffer[BUFFER_SIZE];
    int stringBytesRead = recv(clientSocket, stringBuffer, BUFFER_SIZE, 0);

    if (stringBytesRead > 0) {
        stringBuffer[stringBytesRead] = '\0'; // Null-terminate the string
        std::cout << "Received string from client: " << stringBuffer << std::endl;
    }
    else {
        std::cerr << "Error receiving string data" << std::endl;
    } 
    CStringW receievedString(stringBuffer);
    return receievedString;
}
