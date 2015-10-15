/*
 * FireDetector.h
 *
 *  Created on: Oct 15, 2015
 *      Author: quyen
 */

#ifndef FIREDETECTOR_H_
#define FIREDETECTOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include <pthread.h>
#include <sys/time.h>       /* for set itimer */
#include <unistd.h>     /* for pause */
#include <iostream>

using namespace cv;

namespace TestAlgorithm {



class FireDetector {
public:
	FireDetector();
	virtual ~FireDetector();

	int getFirePixelNumber(Mat aFrame);

	VideoCapture capture;
	Mat frame;
	Mat back;
	Mat front;
	BackgroundSubtractorMOG2 bg;
	void cvShowManyImages(std::string title, int s_cols, int s_rows, int nArgs, ...);

	bool isFire;
};

} /* namespace TestAlgorithm */

#endif /* FIREDETECTOR_H_ */
