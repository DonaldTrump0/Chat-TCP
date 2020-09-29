#include "pch.h"
#include "CTCPServer.h"
#include "Chat.h"
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	SOCKET s = (SOCKET)lpParameter;
	char szBuf[1024];

	while (true)
	{
		// 没有数据，线程阻塞
		int nRecvBytes = recv(s, szBuf, sizeof(szBuf), 0);
		if (SOCKET_ERROR == nRecvBytes)
		{
			CTCPServer::ShowErrMsg("recvfrom error");
			break;
		}

		// 处理数据
		HWND hWnd = theApp.GetMainWnd()->m_hWnd;
		COPYDATASTRUCT data;
		data.lpData = szBuf;
		data.cbData = nRecvBytes;
		data.dwData = 0;
		SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&data);
	}

	return 0;
}

CTCPServer::CTCPServer()
	 : m_socket(NULL)
{
	// 初始化套接字库
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
		ShowErrMsg("WSAStartup error");
		return;
	}

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket)
	{
		ShowErrMsg("server socket init error");
		return;
	}

	// 连接服务器(三次握手)
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(REMOTE_SERVER_PORT);
	inet_pton(AF_INET, REMOTE_SERVER_IP, &serverAddr.sin_addr);
	if (connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == INVALID_SOCKET)
	{
		printf("socket connect error: 0x%08X\n", WSAGetLastError());
		return;
	}

	CreateThread(NULL, 0, &ThreadProc, (LPVOID)m_socket, 0, NULL);
}

CTCPServer::~CTCPServer()
{
	if (m_socket != NULL)
	{
		closesocket(m_socket);
	}

	// 释放资源
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
	TRACE("%s\n", szPre);
	TRACE("0x%X: %s\n", WSAGetLastError(), lpMsgBuf);
	// Free the buffer.
	LocalFree(lpMsgBuf);
}

void CTCPServer::Send(const char* szBuf, int nBufLen)
{
	if (SOCKET_ERROR == send(m_socket, szBuf, nBufLen, 0))
	{
		ShowErrMsg("send error");
	}
}
