/*
 * Producer.cpp
 *
 *  Created on: Oct 6, 2015
 *      Author: quyen
 */

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "Producer.h"

namespace TestBuffer {

Producer::Producer() {
	// TODO Auto-generated constructor stub
	warehouse = NULL;
}

Producer::~Producer() {
	// TODO Auto-generated destructor stub
}

void Producer::initProducer() {
}

void Producer::startProducer() {
	if (pthread_create(&producerThread, NULL, Producer::run, (void*) this)!= 0) {
		std::cout << "Fail to create producerThread" << std::endl;
	}
	std::cout << "[producerThread]Start" << std::endl;
}

void Producer::joinProducer() {
	pthread_join(producerThread, NULL);
}

void* Producer::run(void* arg) {
	Producer* producer = (Producer*) arg;

	while (true) {
		int result = producer->makeProduct();
		usleep(600000);
		//if(result){
//			usleep(200000);
//		}
//		else{
//			sleep(2);
//		}
	}
	return NULL;
}

void Producer::registerWarehouse(Warehouse* aWarehouse) {
	warehouse = aWarehouse;
}

int Producer::makeProduct() {
	int value, result;
	srand(time(NULL));
	value = rand() % 10;
	result = warehouse->storeProduct(value);
	std::cout << "[Producer]Store: " << value << std::endl;
	return result;
}

} /* namespace TestBuffer */
