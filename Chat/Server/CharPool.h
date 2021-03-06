/*
 * MPool.h
 *
 *  Created on: 2019. 1. 17.
 *      Author: choiis1207
 */

#ifndef CHARPOOL_H_
#define CHARPOOL_H_

#include <iostream>
#include <winsock2.h>
#include "ConcurrentQueue.h"
#include "common.h"

#define BLOCK_SIZE 1024

class CharPool {
private:
	char* data;
	ConcurrentQueue<char*> poolQueue;
	DWORD len;
	CharPool();
	~CharPool();
	CharPool(const CharPool& pool);
	CharPool& operator=(const CharPool& pool);
	static CharPool* instance; // Singleton Instance
public:
	// Singleton Instance 를 반환
	static CharPool* getInstance() {
		if (instance == nullptr) {
			cout << "char Memory 50000 Piece!" << endl;
			instance = new CharPool();
		}
		return instance;
	}
	// 메모리풀 할당
	char* Malloc();
	// 메모리풀 반환
	void Free(char* freePoint);

};

#endif /* CHARPOOL_H_ */
