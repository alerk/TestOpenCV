/*
 * Producer.h
 *
 *  Created on: Oct 6, 2015
 *      Author: quyen
 */

#ifndef PRODUCER_H_
#define PRODUCER_H_

#include "Warehouse.h"

namespace TestBuffer {

class Producer {
public:
	Producer();
	virtual ~Producer();

	void initProducer();
	void startProducer();
	void joinProducer();
	static void* run(void* arg);

	void registerWarehouse(Warehouse* aWarehouse);
	int makeProduct();
private:
	pthread_t producerThread;
	Warehouse* warehouse;

};

} /* namespace TestBuffer */

#endif /* PRODUCER_H_ */
