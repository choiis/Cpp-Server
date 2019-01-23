/*
 * common.h
 *
 *  Created on: 2019. 1. 8.
 *      Author: choiis1207
 */

#ifndef COMMON_H_
#define COMMON_H_

using namespace std;
#define SIZE 256
#define BUF_SIZE 512
#define NAME_SIZE 20

// CP가 Recv 받을때 READ Send 받을때 WRITE
#define READ 6
#define WRITE 7
#define READ_MORE 8

// 클라이언트 상태 정보 => 서버에서 보관할것
#define STATUS_INIT 0
#define STATUS_LOGOUT 1
#define STATUS_WAITING 2
#define STATUS_CHATTIG 3
#define STATUS_WHISPER 4

// 서버에게 지시 사항
#define USER_MAKE 1
#define USER_ENTER 2
#define USER_LIST 3
#define USER_OUT 4

// 서버에 올 지시 사항
#define ROOM_MAKE 6
#define ROOM_ENTER 7
#define ROOM_OUT 8
#define WHISPER 9
#define ROOM_INFO 10
#define ROOM_USER_INFO 11
// 로그인 중복 방지
#define NOW_LOGOUT 0
#define NOW_LOGIN 1

#define loginBeforeMessage "1.계정생성 2.로그인하기 3.계정 리스트 4.종료 5.콘솔지우기"
#define waitRoomMessage	"1.방 정보 보기 2.방 만들기 3.방 입장하기 4.유저 정보 5.귓속말 6.로그아웃 7.콘솔지우기"
#define errorMessage "잘못된 명령어 입니다\n"
#define chatRoomMessage  "나가시려면 \\out을 콘솔지우기는 \\clear를 입력하세요"

// 실제 통신 패킷 데이터
typedef struct {
	int bodySize;
	int clientStatus;
	int direction;
	char *message;
} PACKET_DATA, *P_PACKET_DATA;

// 비동기 통신에 필요한 구조체
typedef struct { // buffer info
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[SIZE];  // 버퍼에 PACKET_DATA가 들어갈 것이므로 크기 맞춤
	int serverMode;
	DWORD recvByte; // 지금까지 받은 바이트를 알려줌, 지금까지 보낸 사람
	DWORD totByte; //전체 받을 바이트를 알려줌, 총 보낼 사람
	int bodySize; // 패킷의 body 사이즈
	char *recvBuffer;
} PER_IO_DATA, *LPPER_IO_DATA;


#endif /* COMMON_H_ */
