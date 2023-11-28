
// server.cpp

#include "pch.h"
#include "Server.h"

Server::Server() {
    Rflag = -1;//no input
    Manager_com_flag = -1;
    Manager_Req_flag = 0;
    image_upload_flag = false;
    WSAStartup(MAKEWORD(2, 2), &wsaData);//Windows ���� ���α׷��ֿ��� Winsock ���̺귯���� �ʱ�ȭ

    // TCP socket ����
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    std::cout << "Server listening on port " << PORT << "..." << std::endl;
}
Server::Server(const int _port) {

    WSAStartup(MAKEWORD(2, 2), &wsaData);//Windows ���� ���α׷��ֿ��� Winsock ���̺귯���� �ʱ�ȭ

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);
    image_upload_flag = false;
    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    std::cout << "Server listening on port " << _port << "..." << std::endl;
}


Server::~Server() {
    closesocket(serverSocket);
    WSACleanup();
}

void Server::run(CString& received_string) {
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
<<<<<<< HEAD
    SOCKET clientSocket = accept(serverSocket, 
        (struct sockaddr*)&clientAddr, &clientAddrLen);
=======
    // ���� 1 accept �� ��� ������ ���� ������ΰ� ����.
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
>>>>>>> ee5be685fee14cf44e607df5b63e19b2d0702249


    // Receive data type header
    DataType dataType;
    int headerBytesRead = recv(clientSocket, 
        reinterpret_cast<char*>(&dataType), sizeof(DataType), 0);


    if (headerBytesRead != sizeof(DataType)) {
        std::cerr << "Error reading data type header" << std::endl;
        closesocket(clientSocket);
    }

    if (dataType == IMAGE_REC) //0
    {
        receiveImage(clientSocket);
        set_Rflag(0);
    }
    else if (dataType == STRING)// 1 
    {
        received_string = receiveString(clientSocket);
        set_Rflag(1);
    }
    else if (dataType == AUTH_LOG)// 4
    {
        received_string = receiveString(clientSocket);
        set_Rflag(4);
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
void Server::set_Manager_Req_flag(int Manager_Req_flag) {
    this->Manager_Req_flag = Manager_Req_flag;
}

int Server::get_Manager_Req_flag() {
    return this->Manager_Req_flag;
}
void Server::set_Manager_com_flag(int Manager_com_flag) {
    this->Manager_com_flag = Manager_com_flag;
}

int Server::get_Manager_com_flag() {
    return this->Manager_com_flag;
}

void Server::set_image_upload_flag(BOOL image_upload_flag/*receieve flag*/) {
    this->image_upload_flag = image_upload_flag;
}

BOOL Server::get_image_upload_flag() {
    return this->image_upload_flag;
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

    return bytesRead == 0; // ������ ��Ŷ�� �ƴ϶�� false ��ȯ
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

    std::ifstream imageFileSize(image_Path, std::ios::binary | std::ios::ate);// �̹��� ������ ũ�⸦ ������
    int imageSize = static_cast<int>(imageFileSize.tellg());
    imageFileSize.close();

    DataType img_size_Transmission = IMAGE_SIZE;//3
    send(clientSocket, reinterpret_cast<char*>(&img_size_Transmission), sizeof(DataType), 0);

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
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading image file: " << e.what() << std::endl;
    }
    imageFile.close();
    std::cout << "Image sent to client" << std::endl;
    closesocket(clientSocket); // �̹����� ������ �Ŀ� ������ �ݽ��ϴ�.
}


void Server::run_manager() {

    SOCKET clientSocket=NULL;
    if (Manager_Req_flag == 0)//��� ���� ��� listening
    {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)
            &clientAddr, &clientAddrLen);

        ManagerDataType managerDataType;
        int headerBytesRead = recv(clientSocket, reinterpret_cast<char*>
            (&managerDataType), sizeof(ManagerDataType), 0);


        if (headerBytesRead != sizeof(ManagerDataType)) {
            std::cerr << "Error reading data type header" << std::endl;
            closesocket(clientSocket);
        }
        if (managerDataType == M_IMAGE_REC) //0
        {
            std::cout << "Manager REQUEST" << std::endl;
            receiveImage(clientSocket);
            image_upload_flag = true;
            Manager_Req_flag = 1; // flag �������� �޽��� �ڽ� pop up
        }
    }
    else if (Manager_Req_flag == 2) {
        std::cout << "Manager_Req_flag == 2" << std::endl;
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)
            &clientAddr, &clientAddrLen);
        if (clientSocket != NULL)
        {
            send_comm_manager(clientSocket);
        }
        Manager_Req_flag = 0;
    }
}

void Server::send_comm_manager(SOCKET clientSocket) {
    
    // flag ��� ������ msg �ڽ����� Y->setflag=1 N->setflag=0
    std::cout << "send_comm_manager" << std::endl;

    if (Manager_com_flag==1)//server Ŭ������ ���� �޽��� �ڽ� ��ư�� setflag�� �ִ���
    {
        ManagerDataType managerDataType = OPEN;//3
        send(clientSocket, reinterpret_cast<char*>(&managerDataType), sizeof(ManagerDataType), 0);
        std::cout << "Door OPEN" << std::endl;
        //std::string log=receiveString(clientSocket);
        Manager_com_flag = -1;
    }
    else if(Manager_com_flag == 0)
    {
        ManagerDataType managerDataType = CLOSE;//4
        send(clientSocket, reinterpret_cast<char*>(&managerDataType), sizeof(ManagerDataType), 0);
        std::cout << "Door CLOSE" << std::endl;
        //std::string log = receiveString(clientSocket);
        Manager_com_flag = -1;
    }

}



/*
void Server::sendAck(SOCKET clientSocket) {
    DataType ackType = ACK;//9�� �����ִ°���.
    send(clientSocket, reinterpret_cast<char*>(&ackType), sizeof(DataType), 0);
    //������ DataType�� ACK=9�� �߰��������.
}
*/
