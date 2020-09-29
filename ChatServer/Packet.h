#pragma once
#include<string>
using namespace std;

enum PACKET_CMD
{
    // 发给服务端的命令
    S_CMD_LOGIN,
    S_CMD_LOGOUT,
    S_CMD_CHAT,

    // 发给客户端的命令
    C_CMD_PRIVATE_CHAT,
    C_CMD_BROADCAST,
    C_CMD_SET_USER_LIST,
    C_CMD_ADD_USER,
    C_CMD_DELETE_USER
};

struct PacketHeader
{
    short m_snCmd;

    PacketHeader(short snCmd) : m_snCmd(snCmd) {}
};

struct LoginPacket : public PacketHeader
{
    char m_szName[64];

    LoginPacket(short snCmd, const char* szName) 
        : PacketHeader(snCmd)
    { strcpy(m_szName, szName); }

    int Length() { return sizeof(PacketHeader) + strlen(m_szName) + 1; }
};

struct UserInfoPacket : public PacketHeader
{
    short m_snId;
    char m_szName[64];

    UserInfoPacket(short snCmd, short snId, const char* szName)
        : PacketHeader(snCmd),
          m_snId(snId)
    { strcpy(m_szName, szName); }

    int Length() { return sizeof(PacketHeader) + sizeof(m_snId) + strlen(m_szName) + 1; }
};

struct UserInfo
{
    short m_snId;
    char m_szName[64];

    void SetName(const char* szName) { strcpy(m_szName, szName); }

    int Length() { return sizeof(m_snId) + strlen(m_szName) + 1; }

    UserInfo* Next() { return (UserInfo*)(((int)this) + Length()); }
};

// UserInfo变长
struct UserInfoListPacket : public PacketHeader
{
    short m_snCurId;    // 登录用户的ID
    short m_snCnt;
    UserInfo m_aryUserInfo[16];     // 提供1024字节左右的缓存空间

    UserInfoListPacket(short snCmd, short snCurId, short snCnt)
        : PacketHeader(snCmd),
          m_snCurId(snCurId),
          m_snCnt(snCnt) {}

    int Length()
    {
        int nLen = sizeof(PacketHeader) + sizeof(m_snCurId) + sizeof(m_snCnt);
        UserInfo* pUserInfo = m_aryUserInfo;
        for (int i = 0; i < m_snCnt; i++)
        {
            nLen += pUserInfo->Length();
            pUserInfo = pUserInfo->Next();
        }
        return nLen;
    }
};

struct UserIdPacket : public PacketHeader
{
    short m_snId;

    UserIdPacket(short snCmd, short snId)
        : PacketHeader(snCmd),
          m_snId(snId) {}

    int Length() { return sizeof(PacketHeader) + sizeof(m_snId); }
};

struct ChatPacket : public PacketHeader
{
    short m_snFromId;
    short m_snToId;
    char m_szMsg[1024];

    ChatPacket(short snCmd, short snFromId, short snToId, const char* szMsg)
        : PacketHeader(snCmd),
        m_snFromId(snFromId),
        m_snToId(snToId) 
    { strcpy(m_szMsg, szMsg); }

    int Length() { return sizeof(PacketHeader) + sizeof(m_snFromId) + sizeof(m_snToId) + strlen(m_szMsg) + 1; }
};

struct UserMsgPacket : public PacketHeader
{
    short m_snFromId;
    char m_szMsg[1024];

    UserMsgPacket(short snCmd, short snFromId, const char* szMsg)
        : PacketHeader(snCmd),
          m_snFromId(snFromId) 
    { strcpy(m_szMsg, szMsg); }

    int Length() { return sizeof(PacketHeader) + sizeof(m_snFromId) + strlen(m_szMsg) + 1; }
};