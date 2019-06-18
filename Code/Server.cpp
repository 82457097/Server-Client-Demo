#include<iostream>
#include"Server.h"

using std::cerr;
using std::cout;
using std::endl;

#define SERVER_PORT 5000
#define MSG_BUF_SIZE 1024

Server::Server() {
	cout << "��ʼ����......\n";
	//��ʼ��
	winsock_ver = MAKEWORD(2,2);
	addr_len = sizeof(SOCKADDR_IN);
	addr_svr.sin_family = AF_INET;
	addr_svr.sin_port = ::htons(SERVER_PORT); //�������˿�
	addr_svr.sin_addr.S_un.S_addr = ADDR_ANY;
	memset(buf_ip, 0, IP_BUF_SIZE);

	//����������
	ret_val = ::WSAStartup(winsock_ver, &wsa_data);
	if (ret_val != 0) {
		cerr << "WSA�޷�������������룺" << ::WSAGetLastError() << "\n";
		system("pause");
		exit(1);
	}
	cout << "WSA�����ɹ�......\n";

	//�����׽���
	sock_svr = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_svr == INVALID_SOCKET) {
		cerr << "�����������׽���ʧ��,������룺" << ::WSAGetLastError() << "\n";
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "�������׽��ִ����ɹ�......\n";

	//�󶨷�������ַ
	ret_val = ::bind(sock_svr, (SOCKADDR*)&addr_svr, addr_len);
	if (ret_val != 0) {
		cerr << "���׽���ʧ�ܣ�������룺" << ::WSAGetLastError() << "\n";
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "�������׽��ְ󶨳ɹ�......\n";

	//��������
	ret_val = ::listen(sock_svr, SOMAXCONN);
	if (ret_val == SOCKET_ERROR) {
		cerr << "��������ʧ�ܣ�������룺" << WSAGetLastError() << "\n";
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "�������׽��ֿ�ʼ����......\n";

	cout << "�����������ɹ���\n";
}

Server::~Server() {
	::closesocket(sock_svr);
	::closesocket(sock_clt);
	::WSACleanup();
	cout << "�ر��׽��ֳɹ���" << endl;
}

DWORD WINAPI createClientThread(LPVOID lpParameter);

void Server::waitForClient() {
	while (true) {
		sock_clt = ::accept(sock_svr, (SOCKADDR*)&addr_clt, &addr_len);
		if (sock_clt == INVALID_SOCKET) {
			cerr << "�޷����ܿͻ��ˣ�������룺" << WSAGetLastError() << endl;
			::WSACleanup();
			system("pause");
			exit(1);
		}
		::InetNtop(addr_clt.sin_family, &addr_clt, buf_ip, IP_BUF_SIZE);
		cout << "һ���µĿͻ�������...IP��ַ��" << buf_ip << "�˿ں��룺" << ::ntohs(addr_clt.sin_port) << endl;
		h_thread = ::CreateThread(nullptr, 0, createClientThread, (LPVOID)sock_clt, 0, nullptr);
		if (h_thread == NULL) {
			cerr << "�޷������̣߳�������룺" << WSAGetLastError() << endl;
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
				cout << "�ͻ�������ر�����......" << endl;
				break;
			}
			cout << "Message Received:" << buf_msg << endl;
			snd_result = ::send(sock_clt, buf_msg, MSG_BUF_SIZE, 0);
			if (snd_result == SOCKET_ERROR) {
				cerr << "�޷�������Ϣ���ͻ��ˣ�������룺" << WSAGetLastError() << endl;
				::closesocket(sock_clt);
				system("pause");
				return 1;
			}
		}
		else if (ret_val == 0) {
			cout << "�ر�����......"<<endl;
		}
		else {
			cerr << "�޷��������Կͻ��˵���Ϣ��������룺" << WSAGetLastError() << endl;
			::closesocket(sock_clt);
			system("pause");
			return 1;
		}
	} while (ret_val > 0);

	ret_val = ::shutdown(sock_clt, SD_SEND);
	if (ret_val == SOCKET_ERROR) {
		cerr << "�޷��رտͻ����׽��֣�������룺" << WSAGetLastError() << endl;
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