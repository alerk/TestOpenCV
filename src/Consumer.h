/*
 * Consumer.h
 *
 *  Created on: Oct 6, 2015
 *      Author: quyen
 */

#ifndef CONSUMER_H_
#define CONSUMER_H_

#include "Warehouse.h"

namespace TestBuffer {

class Consumer {
public:
	Consumer();
	virtual ~Consumer();

	void initConsumer();
	void startConsumer();
	void joinConsumer();
	static void* run(void* arg);

	void registerWarehouse(Warehouse* aWarehouse);
	int getProduct();
private:
	pthread_t consumerThread;
	Warehouse* warehouse;
};

} /* namespace TestBuffer */

#endif /* CONSUMER_H_ */
