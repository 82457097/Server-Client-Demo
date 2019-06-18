#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

const char DEFAULT_PORT[] = "5000";
const int SEND_BUF_SIZE = 256;

//�ͻ���
int main() {
	WSADATA wsa_data;//WSADATA����������windows socketִ�е���Ϣ
	int i_result = 0;//���շ���ֵ
	SOCKET sock_client = INVALID_SOCKET;
	addrinfo *result = nullptr, hints;

	//��ʼ��winsock��̬�⣨ws2_32.dll����MAKEWORD(2��2)��������ʹ��winsock2.2�汾
	i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (i_result != 0) {
		cerr << "����WSAStartup()����ʧ�ܣ�" << i_result << "\n";
		system("pause");
		return 1;
	}

	SecureZeroMemory(&hints, sizeof(hints));//��0�����һ���ڴ�����
	hints.ai_family = AF_UNSPEC;//ָ��Э���� ��socket�����ֻ���� AF_INET��AF_UNSPEC ��ַ��
	hints.ai_socktype = SOCK_STREAM;//�׽������� SOCK_STREAM ʹ��OOB���ݴ�������ṩ˳��
	hints.ai_protocol = IPPROTO_TCP;//Э������ IPPROTO_TCP �������Э�飨TCP��

	i_result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);	//getaddrinfo�����ܹ��������ֵ���ַ�Լ����񵽶˿�������ת����
																	//���ص���һ��sockaddr�ṹ�����������һ����ַ�嵥��
	if (i_result != 0) {
		cerr << "getaddrinfo()����ʧ�ܣ�������룺" << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 1;
	}

	//�����׽���
	//socket()������ԭ�����£������������һ��Э����Ϊdomain��Э������Ϊtype��Э����Ϊprotocol���׽����ļ���������
	//����������óɹ����᷵��һ����ʶ����׽��ֵ��ļ���������ʧ�ܵ�ʱ�򷵻�-1��
	sock_client = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock_client == INVALID_SOCKET) {
		cerr << "�׽���()����ʧ�ܣ�������룺" << WSAGetLastError() << "\n";
		WSACleanup();
		system("pause");
		return 1;
	}

	//���ӿͻ���
	//�ͻ���connect�����ͷ�������accept������һ�Ժû��ѣ�����ͻ���û��ȥconnect����ô����˵�acceptҲ�������
	i_result = connect(sock_client, result->ai_addr, result->ai_addrlen);
	if (i_result == SOCKET_ERROR) {
		cerr << "Connect()����ʽʧ�ܣ�������룺" << WSAGetLastError() << "\n";
		WSACleanup();
		system("pause");
		return 1;
	}
	cout << "�ѳɹ����ӷ�����..." << endl;

	//��getaddrinfo���ص����д洢�ռ䶼�Ƕ�̬��ȡ�ģ���Щ�洢�ռ����ͨ������freeaddrinfo���ظ�ϵͳ��
	freeaddrinfo(result);

	char send_buf[SEND_BUF_SIZE];
	int recv_result = 0;

	//��������
	do {
		cout << "��������Ҫ���͵���Ϣ��" << flush;
		cin.getline(send_buf, SEND_BUF_SIZE);
		i_result = send(sock_client, send_buf, static_cast<int>(strlen(send_buf)), 0);
		if (i_result == SOCKET_ERROR) {
			cerr << "send()����ʧ�ܣ�������룺" << WSAGetLastError() << endl;
			closesocket(sock_client);
			WSACleanup();
			system("pause");
			return 1;
		}
		cout << "���͵��ֽ�����" << i_result << endl;

		//���շ��������ص�����
		recv_result = recv(sock_client, send_buf, SEND_BUF_SIZE, 0);
		if (recv_result > 0) {
			cout << "������������" << send_buf << endl;
		}
		else if (recv_result == 0) {
			cout << "�ر�����..." << endl;
		}
		else {
			cerr << "Recv()����ʧ�ܣ�������룺" << WSAGetLastError() << endl;
			closesocket(sock_client);
			WSACleanup();
			system("pause");
			return 1;
		}
	} while (recv_result > 0);
	
	//�ػ�
	i_result = shutdown(sock_client, SD_SEND);//��ֹ��һ���׽ӿ��Ͻ������ݵĽ����뷢��
	if (i_result == SOCKET_ERROR) {
		cerr << "�ػ�()����ʧ�ܣ�������룺" << WSAGetLastError() << "\n";
		closesocket(sock_client);
		WSACleanup();
		system("pause");
		return 1;
	}

	closesocket(sock_client);
	WSACleanup();
	cout << "�ر��׽���..." << endl;
	system("pause");

	return 0;
}