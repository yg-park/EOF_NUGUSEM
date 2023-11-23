
// server.cpp

#include "pch.h"
#include "Server.h"

Server::Server() {
    Rflag = -1;//no input

    WSAStartup(MAKEWORD(2, 2), &wsaData);

<<<<<<< HEAD
    // TCP socket ����
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
=======
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
>>>>>>> 8c957fe9a994d9bea8eaf7342d477b619fcb89c7

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    std::cout << "Server listening on port " << PORT << "..." << std::endl;
}

Server::~Server() {
    closesocket(serverSocket);
    WSACleanup();
}

void Server::run(CString& received_string) {
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    // ���� 1 accept �� ��� ������ ���� ������ΰ� ����.
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

<<<<<<< HEAD

    // Receive data type header
=======
>>>>>>> 8c957fe9a994d9bea8eaf7342d477b619fcb89c7
    DataType dataType;
    int headerBytesRead = recv(clientSocket, reinterpret_cast<char*>(&dataType), sizeof(DataType), 0);


    if (headerBytesRead != sizeof(DataType)) {
        std::cerr << "Error reading data type header" << std::endl;
        closesocket(clientSocket);
    }

    if (dataType == IMAGE) {
        receiveImage(clientSocket);
        sendAck(clientSocket); // �̹��� ���� �Ϸ� �� ACK ����
        set_Rflag(0);
        
    }
    else if (dataType == STRING) {
        //handleImageTransmissionCompleteMessage(); // �̹��� ���� �Ϸ� �޽��� ó��
        received_string = receiveString(clientSocket);
        set_Rflag(1);
        sendAck(clientSocket); // ���ڿ� ���� �Ϸ� �� ACK ����

    }
    else {
        std::cerr << "Unknown data type received" << std::endl;
        closesocket(clientSocket);
        return;
    }

    closesocket(clientSocket);
}

void Server::set_Rflag(int Rflag) {
    this->Rflag = Rflag;
}

int Server::get_Rflag() {
    return this->Rflag;
}




// �̹��� �����͸� �����ߴ��� Ȯ���ϴ� �Լ�
bool Server::isLastPacket(const char* buffer, int bytesRead) {
    // ������ ��Ŷ�� Ȯ���� �� �ִ� ������ �߰�
    // ���⼭�� buffer�� ������ 4����Ʈ�� END_OF_IMAGE_MARKER���� Ȯ��

    if (bytesRead >= sizeof(int)) {
        int marker;
        memcpy(&marker, buffer + bytesRead - sizeof(int), sizeof(int));

        return marker == END_OF_IMAGE_MARKER;
    }

    return false; // ������ ��Ŷ�� �ƴ϶�� false ��ȯ
}



void Server::receiveImage(SOCKET clientSocket) {
    std::ofstream receivedFile("received_image.png", std::ios::binary);
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        receivedFile.write(buffer, bytesRead);
        std::cout << "Received " << bytesRead << " bytes of image data from client" << std::endl;

        // ������ ��Ŷ���� Ȯ��
        if (isLastPacket(buffer, bytesRead)) {
            set_Rflag(0); // �̹��� flag
            std::cout << "Image received and saved as received_image.png" << std::endl;
            
            break;
        }
    }

    receivedFile.close();
}

CString Server::receiveString(SOCKET clientSocket) {
    char stringBuffer[BUFFER_SIZE];
    int stringBytesRead = recv(clientSocket, stringBuffer, BUFFER_SIZE, 0);

    if (stringBytesRead > 0) {
        stringBuffer[stringBytesRead] = '\0';
        std::cout << "Received string from client: " << stringBuffer << std::endl;
    }
    else {
        std::cerr << "Error receiving string data" << std::endl;
    }

    CStringA receivedString(stringBuffer);
    set_Rflag(1);//1:string for log
    return receivedString;
}



void Server::sendImageToClient(CString image_Path) {
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    std::ifstream imageFile(image_Path, std::ios::binary);

    if (!imageFile.is_open()) {
        std::cerr << "Error opening image file: " << image_Path << std::endl;
        closesocket(clientSocket);
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;

    try {
        while ((bytesRead = imageFile.read(buffer, BUFFER_SIZE).gcount()) > 0) {
            int bytesSent = send(clientSocket, buffer, bytesRead, 0);

            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "Error sending image data: " << WSAGetLastError() << std::endl;
                break;
            }

            std::cout << "Sent " << bytesSent << " bytes of image data to client" << std::endl;
        }

        // Ŭ���̾�Ʈ���� �̹��� ������ �Ϸ�Ǿ����� �˸��� �޽��� ����
        DataType endTransmission = STRING;
        send(clientSocket, reinterpret_cast<char*>(&endTransmission), sizeof(DataType), 0);
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading image file: " << e.what() << std::endl;
    }

    imageFile.close();
    std::cout << "Image sent to client" << std::endl;
    closesocket(clientSocket); // �̹����� ������ �Ŀ� ������ �ݽ��ϴ�.
    std::cout << "client Socket Off" << std::endl;
}

void Server::sendImageToClientAsync(CString image_Path) {
    std::cout << "sendImageToClientAsync" << std::endl;
    // �̹����� �����ϴ� ������ ����
    std::thread([this, image_Path]() {
        // imagePathA�� �� �����忡���� ���� ���̹Ƿ� ���纻�� ���
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
       
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error accepting client connection: " << WSAGetLastError() << std::endl;
            return;
        }

        std::ifstream imageFile(image_Path, std::ios::binary);

        if (!imageFile.is_open()) {
            std::cerr << "Error opening image file: " << image_Path << std::endl;
            closesocket(clientSocket);
            return;
        }

        char buffer[BUFFER_SIZE];
        int bytesRead;

        try {
            while ((bytesRead = imageFile.read(buffer, BUFFER_SIZE).gcount()) > 0) {
                int bytesSent = send(clientSocket, buffer, bytesRead, 0);

                if (bytesSent == SOCKET_ERROR) {
                    std::cerr << "Error sending image data: " << WSAGetLastError() << std::endl;
                    break;
                }

                std::cout << "Sent " << bytesSent << " bytes of image data to client" << std::endl;
            }

            // Ŭ���̾�Ʈ���� �̹��� ������ �Ϸ�Ǿ����� �˸��� �޽��� ����
            DataType endTransmission = STRING;
            send(clientSocket, reinterpret_cast<char*>(&endTransmission), sizeof(DataType), 0);
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading image file: " << e.what() << std::endl;
        }

        imageFile.close();
        std::cout << "Image sent to client" << std::endl;

        // �̹��� ������ �Ϸ�� �Ŀ� ������ �ݽ��ϴ�.
        closesocket(clientSocket);
        std::cout << "client Socket Off" << std::endl;

        // �̹��� ������ �Ϸ�� �Ŀ� �߰� �۾��� ������ �� �ֽ��ϴ�.
        handleImageTransmissionCompleteMessage();
        }).detach();
}

void Server::handleImageTransmissionCompleteMessage() {
    std::cout << "Image transmission complete message received from client" << std::endl;
    // �̹��� ���� �Ϸ� ���� �߰� �۾��� ���⿡ �߰��մϴ�.


}

void Server::sendAck(SOCKET clientSocket) {
    DataType ackType = ACK;//9�� �����ִ°���.
    send(clientSocket, reinterpret_cast<char*>(&ackType), sizeof(DataType), 0);
}