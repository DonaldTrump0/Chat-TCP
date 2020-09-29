#include "CTCPServer.h"

int CTCPServer::s_nIdAcc = 1;

// ��������
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    int* pParam = (int*)lpParameter;
    CTCPServer* pTCPServer = (CTCPServer*)pParam[0];
    SOCKET s = (SOCKET)pParam[1];
    char szBuf[1024] = { 0 };

    while (true)
    {
        // ��������
        int nLen = recv(s, szBuf, sizeof(szBuf), 0);
        if (SOCKET_ERROR == nLen)
        {
            pTCPServer->ShowErrMsg("recv error");
            // TODO
            return 0;
        }

        // ����
        PacketHeader* pPacketHeader = (PacketHeader*)szBuf;
        switch (pPacketHeader->m_snCmd)
        {
            // ��¼
        case S_CMD_LOGIN:
            pTCPServer->OnLogin(s, szBuf);
            break;
            // �˳�
        case S_CMD_LOGOUT:
            pTCPServer->OnLogout(s, szBuf);
            return 0;
            // ת����Ϣ
        case S_CMD_CHAT:
            pTCPServer->OnChat(s, szBuf);
            break;
        default:
            break;
        }

        printf("recv len = %d, buf = %s\n", nLen, szBuf);
    }
}

CTCPServer::CTCPServer() : m_ListenSocket(NULL)
{    
    // ��ʼ���׽��ֿ�
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ShowErrMsg("WSAStartup error");
        return;
    }
    
    // ��ʼ���׽���(˵��ʹ�õ�Э��)
    m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == m_ListenSocket)
    {
        ShowErrMsg("server socket init error");
        return;
    }

    // �󶨶˿�(��ϵͳ����һ���˿�)
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5566);  // Сβת��β
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(m_ListenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        ShowErrMsg("bind error");
        return;
    }

    // ����
    if (listen(m_ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen error: 0x%08X\n", WSAGetLastError());
        return;
    }

    while (true)
    {
        // ���ܿͻ�������
        sockaddr_in clientAddr;
        int nLen = sizeof(clientAddr);
        SOCKET s = accept(m_ListenSocket, (sockaddr*)&clientAddr, &nLen);
        printf("accept ip = %s, port = %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        int* pParam = new int[2];
        pParam[0] = (int)this;
        pParam[1] = (int)s;
        // ���������߳�
        CreateThread(NULL, 0, ThreadProc, (LPVOID)pParam, 0, 0);
    }
}

CTCPServer::~CTCPServer()
{
    if (m_ListenSocket != NULL)
    {
        closesocket(m_ListenSocket);
    }

    // �ͷ���Դ
    WSACleanup();
}

void CTCPServer::ShowErrMsg(const char* szPre)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );
    // Display the string.
    printf("%s\n", szPre);
    printf("0x%X: %s\n", WSAGetLastError(), lpMsgBuf);
    // Free the buffer.
    LocalFree(lpMsgBuf);
}

void CTCPServer::OnLogin(SOCKET s, const char* szBuf)
{
    // ����
    LoginPacket* pLoginPacket = (LoginPacket*)szBuf;
    char* szName = pLoginPacket->m_szName;

    // �ͻ�����Ϣ
    ClientInfo clientInfo;
    clientInfo.nId = s_nIdAcc++;
    clientInfo.strName = szName;
    clientInfo.s = s;

    // ���(�û��б�)
    UserInfoListPacket userInfoListPacket(C_CMD_SET_USER_LIST, clientInfo.nId, m_mapClient.size());
    UserInfo* pUserInfo = userInfoListPacket.m_aryUserInfo;
    for (auto entry : m_mapClient)
    {
        pUserInfo->m_snId = entry.first;
        pUserInfo->SetName(entry.second.strName.c_str());
        pUserInfo = pUserInfo->Next();
    }
    // �����û����������û�
    if (SOCKET_ERROR == send(s, (char*)&userInfoListPacket, userInfoListPacket.Length(), 0))
    {
        ShowErrMsg("socket send error");
    }

    // ���(���û�)
    UserInfoPacket userInfoPacket(C_CMD_ADD_USER, clientInfo.nId, szName);
    // �������û����͸��û���Ϣ
    for (auto entry : m_mapClient)
    {
        if (SOCKET_ERROR == send(entry.second.s, (char*)&userInfoPacket, userInfoPacket.Length(), 0))
        {
            ShowErrMsg("socket send error");
        }
    }

    // ����ͻ��˵ķ�������ַ
    m_mapClient[clientInfo.nId] = clientInfo;
}

void CTCPServer::OnLogout(SOCKET s, const char* szData)
{
    UserIdPacket* pUserIdPacket = (UserIdPacket*)szData;
    int nId = pUserIdPacket->m_snId;

    closesocket(s);
    m_mapClient.erase(nId);

    UserIdPacket userIdPacket(C_CMD_DELETE_USER, nId);
    // ֪ͨ���пͻ���
    for (auto entry : m_mapClient)
    {
        send(entry.second.s, (char*)&userIdPacket, userIdPacket.Length(), 0);
    }
}

void CTCPServer::OnChat(SOCKET s, const char* szData)
{
    // ����
    ChatPacket* pChatPacket = (ChatPacket*)szData;
    int nFromId = pChatPacket->m_snFromId;
    int nToId = pChatPacket->m_snToId;
    char* szMsg = pChatPacket->m_szMsg;

    // Ⱥ��
    if (nToId == 0)
    {
        UserMsgPacket userMsgPacket(C_CMD_BROADCAST, nFromId, szMsg);
        for (auto entry : m_mapClient)
        {
            if (entry.first != nFromId)
            {
                send(entry.second.s, (char*)&userMsgPacket, userMsgPacket.Length(), 0);
            }
        }
    }
    // ˽��
    else
    {
        UserMsgPacket userMsgPacket(C_CMD_PRIVATE_CHAT, nFromId, szMsg);
        send(m_mapClient[nToId].s, (char*)&userMsgPacket, userMsgPacket.Length(), 0);
    }
}