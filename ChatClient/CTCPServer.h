#pragma once

#define REMOTE_SERVER_IP "192.168.1.4"
#define REMOTE_SERVER_PORT 5566

class CTCPServer
{
public:
    CTCPServer();
    ~CTCPServer();

    void Send(const char* szBuf, int nBufLen);
    static void ShowErrMsg(const char* szPre);

public:
    SOCKET m_socket;
};