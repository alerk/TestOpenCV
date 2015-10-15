#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <unistd.h>

#include "Warehouse.h"
#include "Producer.h"
#include "Consumer.h"
#include "FireDetector.h"

using namespace std;
using namespace cv;

#define TESTING_ALGORITHM 1

int main() {

#if 0
	TestBuffer::Warehouse warehouse;
	TestBuffer::Producer producer;
	TestBuffer::Consumer consumer;

	producer.initProducer();
	consumer.initConsumer();
	warehouse.initWarehouse();

	producer.registerWarehouse(&warehouse);
	consumer.registerWarehouse(&warehouse);

	warehouse.startWarehouse();
	producer.startProducer();
	consumer.startConsumer();

	warehouse.joinWarehouse();
	//producer.joinProducer();
	//consumer.joinConsumer();
#endif

	while(true)
	{
//		cout << "Main thread" << endl;
		sleep(2);
	}

	return 0;
}
