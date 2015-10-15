/*
 * BufferClass.h
 *
 *  Created on: Oct 6, 2015
 *      Author: quyen
 */

#ifndef WAREHOUSE_H_
#define WAREHOUSE_H_

#include <pthread.h>
#include <queue>

namespace TestBuffer {

class Warehouse {
public:
	Warehouse();
	virtual ~Warehouse();

	void initWarehouse();
	void startWarehouse();
	void joinWarehouse();
	static void* run(void* arg);

	int storeProduct(int value);
	int sendProduct(int &value);
	void listProduct();

	void checkProductStatus();

private:
	pthread_t bufferThread;
	pthread_mutex_t bufferInMutex, bufferOutMutex, bufferMutex;
	pthread_cond_t bufferEmptyCond, bufferFullCond;
	static const int BUFFER_MAX_SIZE = 5;
	int buffer[BUFFER_MAX_SIZE];
	int readPos, writePos, total;
	bool isLockedWrite, isLockedRead;



	bool isBufferEmpty();
	bool isBufferFull();

};

} /* namespace TestBuffer */

#endif /* WAREHOUSE_H_ */
