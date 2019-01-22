//============================================================================
// Name        : Client.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>
#include <regex>
#include "common.h"
#include "MPool.h"
#include "CharPool.h"
// ���� Ŭ���̾�Ʈ ���� => ���� => ���� �α��� �������� �ٲ��
int clientStatus;

CRITICAL_SECTION cs;

using namespace std;

typedef struct { // socket info
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// Recv ��� �����Լ�
void RecvMore(SOCKET sock, LPPER_IO_DATA ioInfo) {
	DWORD recvBytes = 0;
	DWORD flags = 0;
	memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
	ioInfo->wsaBuf.len = SIZE;
	memset(ioInfo->buffer, 0, SIZE);
	ioInfo->wsaBuf.buf = ioInfo->buffer;
	ioInfo->serverMode = READ_MORE; // GetQueuedCompletionStatus ���� �бⰡ Recv�� ���� �ְ�

	// ��� Recv
	WSARecv(sock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags,
			&(ioInfo->overlapped),
			NULL);
}
// Recv �����Լ�
void Recv(SOCKET sock) {
	DWORD recvBytes = 0;
	DWORD flags = 0;
	MPool* mp = MPool::getInstance();
	LPPER_IO_DATA ioInfo = mp->malloc();

	memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
	ioInfo->wsaBuf.len = SIZE;
	ioInfo->wsaBuf.buf = ioInfo->buffer;
	ioInfo->serverMode = READ; // GetQueuedCompletionStatus ���� �бⰡ Recv�� ���� �ְ�
	ioInfo->recvByte = 0;
	ioInfo->totByte = 0;

	// ��� Recv
	WSARecv(sock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags,
			&(ioInfo->overlapped),
			NULL);
}

// WSASend�� call
void SendMsg(SOCKET clientSock, const char* msg, int status, int direction) {
	MPool* mp = MPool::getInstance();
	LPPER_IO_DATA ioInfo = mp->malloc();

	memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));

	int len = strlen(msg) + 1;
	CharPool* charPool = CharPool::getInstance();
	char* packet = charPool->malloc(); // char[len + (3 * sizeof(int))];
	memcpy(packet, &len, 4); // dataSize;
	memcpy(((char*) packet) + 4, &status, 4); // status;
	memcpy(((char*) packet) + 8, &direction, 4); // direction;
	memcpy(((char*) packet) + 12, msg, len); // status;

	ioInfo->wsaBuf.buf = (char*) packet;
	ioInfo->wsaBuf.len = len + (3 * sizeof(int));
	ioInfo->serverMode = WRITE;
	// cout << "send " << msg << " status " << status << " direction " << direction << endl;

	WSASend(clientSock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped),
	NULL);
}

string alpha1 = "abcdefghijklmnopqrstuvwxyz";
string alpha2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
string password = "1234";

// �۽��� ����� ������
unsigned WINAPI SendMsgThread(void *arg) {
	// �Ѿ�� clientSocket�� �޾���
	SOCKET clientSock = *((SOCKET*) arg);

	while (1) {

		int direction = -1;
		int status = -1;
		Sleep(1);
		EnterCriticalSection(&cs);
		if (clientStatus == STATUS_LOGOUT) { // �α��� ����
			LeaveCriticalSection(&cs);
			int randNum3 = (rand() % 2);
			if (randNum3 % 2 == 0) {
				int randNum1 = (rand() % 2);
				int randNum2 = (rand() % 26);
				string nickName = "";
				if (randNum1 == 0) {
					nickName += alpha1.at(randNum2);
				} else {
					nickName += alpha2.at(randNum2);
				}
				string msg1 = nickName;
				msg1.append("\\");
				msg1.append(password); // ��й�ȣ
				msg1.append("\\");
				msg1.append(nickName); // �г���
				direction = USER_MAKE;
				status = STATUS_LOGOUT;

				SendMsg(clientSock, msg1.c_str(), status, direction);
			} else {
				int randNum1 = (rand() % 2);
				int randNum2 = (rand() % 26);
				string nickName = "";
				if (randNum1 == 0) {
					nickName += alpha1.at(randNum2);
				} else {
					nickName += alpha2.at(randNum2);
				}
				string msg2 = nickName;
				msg2.append("\\");
				msg2.append(password); // ��й�ȣ
				direction = USER_ENTER;
				status = STATUS_LOGOUT;
				SendMsg(clientSock, msg2.c_str(), status, direction);
			}

		} else if (clientStatus == STATUS_WAITING) {
			LeaveCriticalSection(&cs);

			int directionNum = (rand() % 10);
			if ((directionNum % 2) == 0) { // 0 2 4 6 8 ������

				int randNum1 = (rand() % 2);
				int randNum2 = (rand() % 4);
				string roomName = "";
				if (randNum1 == 0) {
					roomName += alpha1.at(randNum2);
				} else {
					roomName += alpha2.at(randNum2);
				}
				direction = ROOM_ENTER;
				status = STATUS_WAITING;
				SendMsg(clientSock, roomName.c_str(), status, direction);
			} else if ((directionNum % 3) == 0) { // 3 6 9 �游���
				int randNum1 = (rand() % 2);
				int randNum2 = (rand() % 4);
				string roomName = "";
				if (randNum1 == 0) {
					roomName += alpha1.at(randNum2);
				} else {
					roomName += alpha2.at(randNum2);
				}
				direction = ROOM_MAKE;
				status = STATUS_WAITING;
				SendMsg(clientSock, roomName.c_str(), status, direction);
			} else if (directionNum == 7) { // 7 ������
				direction = ROOM_INFO;
				status = STATUS_WAITING;
				SendMsg(clientSock, "", status, direction);
			} else if (directionNum == 1) { // 1 ��������
				direction = ROOM_USER_INFO;
				status = STATUS_WAITING;
				SendMsg(clientSock, "", status, direction);
			}

		} else if (clientStatus == STATUS_CHATTIG) {
			LeaveCriticalSection(&cs);
			int directionNum = (rand() % 20);
			string msg = "";
			if (directionNum < 18) {
				int randNum1 = (rand() % 2);

				for (int i = 0; i <= (rand() % 60) + 1; i++) {
					int randNum2 = (rand() % 26);
					if (randNum1 == 0) {
						msg += alpha1.at(randNum2);
					} else {
						msg += alpha2.at(randNum2);
					}
				}

			} else if (directionNum == 18) { // 20���� �ѹ� clear
				system("cls");
			} else { // 20���� �ѹ� ����
				msg = "\\out";
			}
			status = STATUS_CHATTIG;
			SendMsg(clientSock, msg.c_str(), status, direction);
		} else {
			LeaveCriticalSection(&cs);
		}

	}
	return 0;
}

// ��Ŷ ������ �б�
void PacketReading(LPPER_IO_DATA ioInfo, DWORD bytesTrans) {
	// IO �Ϸ��� ���� �κ�
	if (READ == ioInfo->serverMode) {
		if (bytesTrans < 4) { // ����� �� �� �о�� ��Ȳ
			memcpy(((char*) &(ioInfo->bodySize)) + ioInfo->recvByte,
					ioInfo->buffer, bytesTrans);
		} else {
			memcpy(&(ioInfo->bodySize), ioInfo->buffer, 4);
			CharPool* charPool = CharPool::getInstance();
			ioInfo->recvBuffer = charPool->malloc(); // char[ioInfo->bodySize + 12]; // BodySize��ŭ ���� �Ҵ�
			memcpy(((char*) ioInfo->recvBuffer), ioInfo->buffer, bytesTrans);
		}
		ioInfo->recvByte += bytesTrans; // ���ݱ��� ���� ������ �� ����
	} else { // �� �б�
		if (ioInfo->recvByte >= 4) { // ��� �� �о���
			memcpy(((char*) ioInfo->recvBuffer) + ioInfo->recvByte,
					ioInfo->buffer, bytesTrans);
			ioInfo->recvByte += bytesTrans; // ���ݱ��� ���� ������ �� ����
		} else { // ����� �� ���о��� ���
			int recv = min(4 - ioInfo->recvByte, bytesTrans);
			memcpy(((char*) &(ioInfo->bodySize)) + ioInfo->recvByte,
					ioInfo->buffer, recv); // ������� ä���
			ioInfo->recvByte += bytesTrans; // ���ݱ��� ���� ������ �� ����
			if (ioInfo->recvByte >= 4) {
				CharPool* charPool = CharPool::getInstance();
				ioInfo->recvBuffer = charPool->malloc(); // char[ioInfo->bodySize + 12]; // BodySize��ŭ ���� �Ҵ�
				memcpy(((char*) ioInfo->recvBuffer) + 4,
						((char*) ioInfo->buffer) + recv, bytesTrans - recv); // ���� ����� �ʿ���� => �̶��� ���� �����͸� ����
			}
		}
	}

	if (ioInfo->totByte == 0 && ioInfo->recvByte >= 4) { // ����� �� �о�� ��Ż ����Ʈ ���� �� �� �ִ�
		ioInfo->totByte = ioInfo->bodySize + 12;
	}
}

// Ŭ���̾�Ʈ���� ���� ������ ������ ����ü ����
char* DataCopy(LPPER_IO_DATA ioInfo, int *status) {
	memcpy(status, ((char*) ioInfo->recvBuffer) + 4, 4); // Status
	CharPool* charPool = CharPool::getInstance();
	char* msg = charPool->malloc(); // char[ioInfo->bodySize];	// Msg
	memcpy(msg, ((char*) ioInfo->recvBuffer) + 12, ioInfo->bodySize);
	// �� ���� �޾����� �Ҵ� ����
	charPool->free(ioInfo->recvBuffer);
	// �޸� ����
	MPool* mp = MPool::getInstance();
	mp->free(ioInfo);

	return msg;
}

// ������ ����� ������
unsigned WINAPI RecvMsgThread(LPVOID hComPort) {

	SOCKET sock;
	DWORD bytesTrans;
	LPPER_IO_DATA ioInfo;

	while (1) {
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD) &sock,
				(LPOVERLAPPED*) &ioInfo, INFINITE);

		if (READ_MORE == ioInfo->serverMode || READ == ioInfo->serverMode) {
			// ������ �б� ����
			PacketReading(ioInfo, bytesTrans);

			if ((ioInfo->recvByte < ioInfo->totByte)
					|| (ioInfo->recvByte < 4 && ioInfo->totByte == 0)) { // ���� ��Ŷ ���� || ��� �� ������ -> ���޾ƾ���

				RecvMore(sock, ioInfo); // ��Ŷ ���ޱ�
			} else {
				int status;
				char *msg = DataCopy(ioInfo, &status);

				// Client�� ���� ���� ���� �ʼ�
				// �������� �ذ����� ����
				if (status == STATUS_LOGOUT || status == STATUS_WAITING
						|| status == STATUS_CHATTIG) {
					EnterCriticalSection(&cs);
					if (clientStatus != status) { // ���� ����� �ܼ� clear
						system("cls");
					}
					clientStatus = status; // clear���� client���º��� ���ش�
					LeaveCriticalSection(&cs);
					cout << msg << endl;
				} else if (status == STATUS_WHISPER) { // �ӼӸ� �����϶��� Ŭ���̾�Ʈ ���º�ȭ ����
					cout << msg << endl;
				}
				CharPool* charPool = CharPool::getInstance();
				charPool->free(msg);
				Recv(sock);
			}
		} else if (WRITE == ioInfo->serverMode) {
			MPool* mp = MPool::getInstance();
			CharPool* charPool = CharPool::getInstance();
			charPool->free(ioInfo->wsaBuf.buf);
			mp->free(ioInfo);
		}

	}
	return 0;
}
int main() {

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	HANDLE sendThread, recvThread;

	// Socket lib�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup() error!");
		exit(1);
	}
	// Overlapped IO���� ������ �����
	// TCP ����Ұ�
	hSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0,
	WSA_FLAG_OVERLAPPED);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// �Ӱ迵�� Object ����
	InitializeCriticalSectionAndSpinCount(&cs, 2000);
	while (1) {

		cout << "��Ʈ��ȣ�� �Է��� �ּ��� :";
		string port;
		cin >> port;

		servAddr.sin_port = htons(atoi(port.c_str()));

		if (connect(hSocket, (SOCKADDR*) &servAddr,
				sizeof(servAddr))==SOCKET_ERROR) {
			printf("connect() error!");
		} else {
			break;
		}
	}
	// Completion Port ����
	HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Completion Port �� ���� ����
	CreateIoCompletionPort((HANDLE) hSocket, hComPort, (DWORD) hSocket, 0);

	// ���� ������ ����
	// ������� RecvMsg�� hComPort CP ������Ʈ�� �Ҵ��Ѵ�
	// RecvMsg���� Recv�� �Ϸ�Ǹ� ������ �κ��� �ִ�
	recvThread = (HANDLE) _beginthreadex(NULL, 0, RecvMsgThread,
			(LPVOID) hComPort, 0,
			NULL);

	// �۽� ������ ����
	// Thread�ȿ��� clientSocket���� Send���ٰŴϱ� ���ڷ� �Ѱ��ش�
	// CP�� Send�� ���� �ȵǾ����� GetQueuedCompletionStatus���� Send �Ϸ�ó�� �ʿ����
	sendThread = (HANDLE) _beginthreadex(NULL, 0, SendMsgThread,
			(void*) &hSocket, 0,
			NULL);

	Recv(hSocket);
	WaitForSingleObject(sendThread, INFINITE);
	WaitForSingleObject(recvThread, INFINITE);
	// �Ӱ迵�� Object ��ȯ
	DeleteCriticalSection(&cs);
	closesocket(hSocket);
	WSACleanup();
	return 0;
}