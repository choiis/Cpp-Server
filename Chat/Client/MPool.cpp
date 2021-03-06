/*
* MPool.cpp
*
*  Created on: 2019. 1. 17.
*      Author: choiis1207
*/

#include "MPool.h"
// 생성자
MPool::MPool() {
	data = (char*)malloc(sizeof(PER_IO_DATA)* 30000);
	DWORD i = 0;
	len = 30000;
	memset((char*)data, 0, sizeof(PER_IO_DATA)* 30000);
	for (i = 0; i < 30000; i++) {
		poolQueue.push(data + (sizeof(PER_IO_DATA)* i));
	}
}
// Singleton Instance
MPool* MPool::instance = nullptr;

MPool::~MPool() {
	char *popPoint;
	while (!poolQueue.empty()) {
		poolQueue.try_pop(popPoint);
	}
	free(data);
}
// 메모리풀 할당
LPPER_IO_DATA MPool::Malloc() {
	if (poolQueue.empty()) { // 더 할당 필요한 경우 2000만큼 추가
		char* nextP = new char[sizeof(PER_IO_DATA)* 2000];
		cout << "MPool More" << endl;
		memset((char*)nextP, 0, sizeof(PER_IO_DATA)* 2000);
		for (DWORD j = 0; j < 2000; j++) {
			poolQueue.push(nextP + (sizeof(PER_IO_DATA)* j));
		}
	}
	char* pointer;
	poolQueue.try_pop(pointer);
	return (LPPER_IO_DATA)pointer;
}
// 메모리풀 반환
void MPool::Free(LPPER_IO_DATA freePoint) { // 반환한 포인터의 idx를 원상복구
	memset((char*)freePoint, 0, sizeof(PER_IO_DATA));
	poolQueue.push((char*)freePoint);
}

