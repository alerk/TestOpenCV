/*
 * Consumer.cpp
 *
 *  Created on: Oct 6, 2015
 *      Author: quyen
 */
#include <iostream>
#include <unistd.h>
#include "Consumer.h"

namespace TestBuffer {

Consumer::Consumer() {
	// TODO Auto-generated constructor stub
	warehouse = NULL;
}

Consumer::~Consumer() {
	// TODO Auto-generated destructor stub
}

void Consumer::initConsumer() {
}

void Consumer::startConsumer() {
	if (pthread_create(&consumerThread, NULL, Consumer::run, (void*) this)!= 0) {
		std::cout << "Fail to create consumerThread" << std::endl;
	}
	std::cout << "[consumerThread]Start" << std::endl;
}

void Consumer::joinConsumer() {
	pthread_join(consumerThread, NULL);
}

void* Consumer::run(void* arg) {
	Consumer* consumer = (Consumer*) arg;
	while (true) {
		int result = consumer->getProduct();
		usleep(900000);
//		if(result){
//			usleep(800000);
//		}
//		else{
//			sleep(1);
//		}
	}
	return NULL;
}

void Consumer::registerWarehouse(Warehouse* aWarehouse) {
	warehouse = aWarehouse;
}

int Consumer::getProduct() {
	int value, result;
	result = warehouse->sendProduct(value);
	std::cout << "[Consumer]Get: " << value << std::endl;
	return result;
}

} /* namespace TestBuffer */
