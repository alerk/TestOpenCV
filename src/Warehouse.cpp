/*
 * Warehouse.cpp
 *
 *  Created on: Oct 6, 2015
 *      Author: quyen
 */

#include <iostream>
#include <unistd.h>
#include "Warehouse.h"

namespace TestBuffer {

Warehouse::Warehouse() {
	// TODO Auto-generated constructor stub
	readPos = writePos = total = 0;
	isLockedRead = isLockedWrite = false;
	bufferEmptyCond = bufferFullCond = PTHREAD_COND_INITIALIZER;
	bufferInMutex = bufferOutMutex = PTHREAD_MUTEX_INITIALIZER;
}

Warehouse::~Warehouse() {
	// TODO Auto-generated destructor stub
}

void Warehouse::initWarehouse() {
}

void Warehouse::startWarehouse() {
	if (pthread_create(&bufferThread, NULL, Warehouse::run, (void*) this)
			!= 0) {
		std::cout << "Fail to create bufferThread" << std::endl;
	}
	std::cout << "[BufferThread]Start" << std::endl;
}

void Warehouse::joinWarehouse() {
	pthread_join(bufferThread, NULL);
}

void* Warehouse::run(void* arg) {
	Warehouse* warehouse = (Warehouse*) arg;
	while (true) {
		warehouse->listProduct();
		warehouse->checkProductStatus();
		sleep(1);
	}
	return NULL;
}

int Warehouse::storeProduct(int value) {
	pthread_mutex_lock(&bufferInMutex);
	while(isBufferFull()) {
		std::cout << "Warehouse is full, stop ship in" << std::endl;
		isLockedWrite = true;
		pthread_cond_wait(&bufferFullCond, &bufferInMutex);
	}


	buffer[writePos] = value;
	writePos = (writePos+1) % BUFFER_MAX_SIZE;
	total++;
	pthread_mutex_unlock(&bufferInMutex);

	return 1;
}

int Warehouse::sendProduct(int& value) {
	pthread_mutex_lock(&bufferOutMutex);
	while (isBufferEmpty()) {
		std::cout << "Warehouse is empty, stop ship out" << std::endl;
		isLockedRead = true;
		pthread_cond_wait(&bufferEmptyCond, &bufferOutMutex);
	}


	value = buffer[readPos];
	readPos = (readPos+1)%BUFFER_MAX_SIZE;
	total--;
	pthread_mutex_unlock(&bufferOutMutex);
	return 1;
}

bool Warehouse::isBufferEmpty() {
	return (total==0);
}

bool Warehouse::isBufferFull() {
	return (total==BUFFER_MAX_SIZE);
}

void Warehouse::listProduct() {
	if(isBufferEmpty()){
		std::cout << "[Warehouse]Empty" << std::endl;
		return;
	}

	std::cout << "[Warehouse]Items: ";
	int startPos = readPos;
	int tempTotal = total;

	while(tempTotal>1){
		std::cout << buffer[startPos] << " - ";
		startPos = (startPos+1)%BUFFER_MAX_SIZE;
		tempTotal--;
	}

	std::cout << buffer[startPos] << std::endl;
}

void Warehouse::checkProductStatus() {
	if(isLockedWrite && !isBufferFull()){
		pthread_cond_signal(&bufferFullCond);
		isLockedWrite = false;
	}
	if(isLockedRead && !isBufferEmpty()){
		pthread_cond_signal(&bufferEmptyCond);
		isLockedRead = false;
	}
}

} /* namespace TestBuffer */


