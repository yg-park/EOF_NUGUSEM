// server.h

#pragma once

#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <atlstr.h>
#include <mutex> // �߰��� ���
#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 8888;
constexpr int BUFFER_SIZE = 1024;
#define END_OF_IMAGE_MARKER 9
enum DataType {
    IMAGE = 0,
    STRING = 1,
    ACK = 9, 
};

class Server {
public:
    Server();
    ~Server();
    void run(CString& received_string);
    void set_Rflag(int Rflag/*receieve flag*/);
    int get_Rflag();
    void receiveImage(SOCKET clientSocket);
    CString receiveString(SOCKET clientSocket);
    void sendImageToClient(CString image_Path);
    void sendImageToClientAsync(CString image_Path); // �񵿱������� �̹����� Ŭ���̾�Ʈ�� ������ �Լ�
    void handleImageTransmissionCompleteMessage();
    void sendAck(SOCKET clientSocket); // ACK �۽�
    bool isLastPacket(const char* buffer, int bytesRead);
private:
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    int Rflag; /*receieve flag 0: image 1:string for log 2:RFID_UID*/ 
};
