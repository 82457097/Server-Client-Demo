#include<iostream>
#include"Server.h"

using std::cerr;
using std::cout;
using std::endl;

#define SERVER_PORT 5000
#define MSG_BUF_SIZE 1024

Server::Server() {
	cout << "初始化中......\n";
	//初始化
	winsock_ver = MAKEWORD(2,2);
	addr_len = sizeof(SOCKADDR_IN);
	addr_svr.sin_family = AF_INET;
	addr_svr.sin_port = ::htons(SERVER_PORT); //服务器端口
	addr_svr.sin_addr.S_un.S_addr = ADDR_ANY;
	memset(buf_ip, 0, IP_BUF_SIZE);

	//启动服务器
	ret_val = ::WSAStartup(winsock_ver, &wsa_data);
	if (ret_val != 0) {
		cerr << "WSA无法启动，错误代码：" << ::WSAGetLastError() << "\n";
		system("pause");
		exit(1);
	}
	cout << "WSA启动成功......\n";

	//创建套接字
	sock_svr = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_svr == INVALID_SOCKET) {
		cerr << "创建服务器套接字失败,错误代码：" << ::WSAGetLastError() << "\n";
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "服务器套接字创建成功......\n";

	//绑定服务器地址
	ret_val = ::bind(sock_svr, (SOCKADDR*)&addr_svr, addr_len);
	if (ret_val != 0) {
		cerr << "绑定套接字失败，错误代码：" << ::WSAGetLastError() << "\n";
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "服务器套接字绑定成功......\n";

	//启动监听
	ret_val = ::listen(sock_svr, SOMAXCONN);
	if (ret_val == SOCKET_ERROR) {
		cerr << "监听启动失败，错误代码：" << WSAGetLastError() << "\n";
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "服务器套接字开始监听......\n";

	cout << "服务器启动成功！\n";
}

Server::~Server() {
	::closesocket(sock_svr);
	::closesocket(sock_clt);
	::WSACleanup();
	cout << "关闭套接字成功！" << endl;
}

DWORD WINAPI createClientThread(LPVOID lpParameter);

void Server::waitForClient() {
	while (true) {
		sock_clt = ::accept(sock_svr, (SOCKADDR*)&addr_clt, &addr_len);
		if (sock_clt == INVALID_SOCKET) {
			cerr << "无法接受客户端，错误代码：" << WSAGetLastError() << endl;
			::WSACleanup();
			system("pause");
			exit(1);
		}
		::InetNtop(addr_clt.sin_family, &addr_clt, buf_ip, IP_BUF_SIZE);
		cout << "一个新的客户端连接...IP地址：" << buf_ip << "端口号码：" << ::ntohs(addr_clt.sin_port) << endl;
		h_thread = ::CreateThread(nullptr, 0, createClientThread, (LPVOID)sock_clt, 0, nullptr);
		if (h_thread == NULL) {
			cerr << "无法创建线程，错误代码：" << WSAGetLastError() << endl;
			::WSACleanup();
			system("pause");
			exit(1);
		}
		::CloseHandle(h_thread);
	}
}

DWORD WINAPI createClientThread(LPVOID lpParameter) {
	SOCKET sock_clt = (SOCKET)lpParameter;
	char buf_msg[MSG_BUF_SIZE];
	int ret_val = 0;
	int snd_result = 0;
	do {
		memset(buf_msg, 0, MSG_BUF_SIZE);
		ret_val = ::recv(sock_clt, buf_msg, MSG_BUF_SIZE, 0);
		if (ret_val > 0) {
			if (strcmp(buf_msg, "exit") == 0) {
				cout << "客户端请求关闭连接......" << endl;
				break;
			}
			cout << "Message Received:" << buf_msg << endl;
			snd_result = ::send(sock_clt, buf_msg, MSG_BUF_SIZE, 0);
			if (snd_result == SOCKET_ERROR) {
				cerr << "无法发送消息至客户端，错误代码：" << WSAGetLastError() << endl;
				::closesocket(sock_clt);
				system("pause");
				return 1;
			}
		}
		else if (ret_val == 0) {
			cout << "关闭连接......"<<endl;
		}
		else {
			cerr << "无法接收来自客户端的信息，错误代码：" << WSAGetLastError() << endl;
			::closesocket(sock_clt);
			system("pause");
			return 1;
		}
	} while (ret_val > 0);

	ret_val = ::shutdown(sock_clt, SD_SEND);
	if (ret_val == SOCKET_ERROR) {
		cerr << "无法关闭客户端套接字，错误代码：" << WSAGetLastError() << endl;
		::closesocket(sock_clt);
		system("pause");
		return 1;
	}
	return 0;
}

int main() {
	Server svr;
	svr.waitForClient();

	system("pause");
	return 0;
}