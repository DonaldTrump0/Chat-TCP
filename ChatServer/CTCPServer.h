#pragma once
#include <iostream>
#include <string>
#include <map>
#include <time.h>
#include <ws2tcpip.h>
#include "Packet.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class CTCPServer
{
private:
    struct ClientInfo
    {
        int nId;
        string strName;
        SOCKET s;
    };

public:
    CTCPServer();
    ~CTCPServer();

public:
    void ShowErrMsg(const char* szPre);
    void OnLogin(SOCKET s, const char* szMsg);
    void OnLogout(SOCKET s, const char* szMsg);
    void OnChat(SOCKET s, const char* szMsg);

public:
    SOCKET m_ListenSocket;              // 监听套接字
    map<int, ClientInfo> m_mapClient;   // 客户端map
    static int s_nIdAcc;                // 用户ID累加器
};

