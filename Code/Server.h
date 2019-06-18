#pragma once

#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<ws2tcpip.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#define IP_BUF_SIZE 128

class Server {
public:
	Server();
	~Server();
	Server(const Server &) = delete;
	Server & operator=(const Server &) = delete;
	void waitForClient();


private:
	WORD winsock_ver;
	WSADATA wsa_data;
	SOCKET sock_svr;
	SOCKET sock_clt;
	HANDLE h_thread;
	SOCKADDR_IN addr_svr;
	SOCKADDR_IN addr_clt;
	int ret_val;
	int addr_len;
	char buf_ip[IP_BUF_SIZE];
};
