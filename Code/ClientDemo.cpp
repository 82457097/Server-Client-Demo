#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

const char DEFAULT_PORT[] = "5000";
const int SEND_BUF_SIZE = 256;

//客户端
int main() {
	WSADATA wsa_data;//WSADATA变量，包含windows socket执行的信息
	int i_result = 0;//接收返回值
	SOCKET sock_client = INVALID_SOCKET;
	addrinfo *result = nullptr, hints;

	//初始化winsock动态库（ws2_32.dll），MAKEWORD(2，2)用于请求使用winsock2.2版本
	i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (i_result != 0) {
		cerr << "启动WSAStartup()函数失败：" << i_result << "\n";
		system("pause");
		return 1;
	}

	SecureZeroMemory(&hints, sizeof(hints));//用0来填充一块内存区域
	hints.ai_family = AF_UNSPEC;//指代协议族 在socket编程中只能是 AF_INET，AF_UNSPEC 地址族
	hints.ai_socktype = SOCK_STREAM;//套接字类型 SOCK_STREAM 使用OOB数据传输机制提供顺序
	hints.ai_protocol = IPPROTO_TCP;//协议类型 IPPROTO_TCP 传输控制协议（TCP）

	i_result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);	//getaddrinfo函数能够处理名字到地址以及服务到端口这两种转换，
																	//返回的是一个sockaddr结构的链表而不是一个地址清单。
	if (i_result != 0) {
		cerr << "getaddrinfo()函数失败，错误编码：" << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 1;
	}

	//创建套接字
	//socket()函数的原型如下，这个函数建立一个协议族为domain、协议类型为type、协议编号为protocol的套接字文件描述符。
	//如果函数调用成功，会返回一个标识这个套接字的文件描述符，失败的时候返回-1。
	sock_client = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock_client == INVALID_SOCKET) {
		cerr << "套接字()函数失败，错误代码：" << WSAGetLastError() << "\n";
		WSACleanup();
		system("pause");
		return 1;
	}

	//连接客户端
	//客户端connect函数和服务器的accept函数是一对好基友，如果客户端没有去connect，那么服务端的accept也不会调用
	i_result = connect(sock_client, result->ai_addr, result->ai_addrlen);
	if (i_result == SOCKET_ERROR) {
		cerr << "Connect()函数式失败，错误代码：" << WSAGetLastError() << "\n";
		WSACleanup();
		system("pause");
		return 1;
	}
	cout << "已成功连接服务器..." << endl;

	//由getaddrinfo返回的所有存储空间都是动态获取的，这些存储空间必须通过调用freeaddrinfo返回给系统。
	freeaddrinfo(result);

	char send_buf[SEND_BUF_SIZE];
	int recv_result = 0;

	//发送数据
	do {
		cout << "请您输入要发送的消息：" << flush;
		cin.getline(send_buf, SEND_BUF_SIZE);
		i_result = send(sock_client, send_buf, static_cast<int>(strlen(send_buf)), 0);
		if (i_result == SOCKET_ERROR) {
			cerr << "send()函数失败，错误代码：" << WSAGetLastError() << endl;
			closesocket(sock_client);
			WSACleanup();
			system("pause");
			return 1;
		}
		cout << "发送的字节数：" << i_result << endl;

		//接收服务器返回的数据
		recv_result = recv(sock_client, send_buf, SEND_BUF_SIZE, 0);
		if (recv_result > 0) {
			cout << "服务器反馈：" << send_buf << endl;
		}
		else if (recv_result == 0) {
			cout << "关闭连接..." << endl;
		}
		else {
			cerr << "Recv()函数失败，错误代码：" << WSAGetLastError() << endl;
			closesocket(sock_client);
			WSACleanup();
			system("pause");
			return 1;
		}
	} while (recv_result > 0);
	
	//关机
	i_result = shutdown(sock_client, SD_SEND);//禁止在一个套接口上进行数据的接收与发送
	if (i_result == SOCKET_ERROR) {
		cerr << "关机()函数失败，错误代码：" << WSAGetLastError() << "\n";
		closesocket(sock_client);
		WSACleanup();
		system("pause");
		return 1;
	}

	closesocket(sock_client);
	WSACleanup();
	cout << "关闭套接字..." << endl;
	system("pause");

	return 0;
}