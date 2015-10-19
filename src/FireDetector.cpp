/*
 * FireDetector.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: quyen
 */
#include <stdio.h>
#include <vector>
#include <stdarg.h>
#include "FireDetector.h"

namespace TestAlgorithm {
static int fireThreshold = 10;

const unsigned int  GAP = 10;
const unsigned int  SIZE_1 = 360;
const unsigned int  SIZE_2 = 320;
const unsigned int  SIZE_3 = 280;
const unsigned int DELAY_TIME_MS = 30;

FireDetector::FireDetector() {
	// TODO Auto-generated constructor stub
	sourceName = "Test";
	isFire = false;

}

FireDetector::~FireDetector() {
	// TODO Auto-generated destructor stub
}

void FireDetector::init()
{
	capture = cv::VideoCapture("/home/quyen/work/eclipse/workspace/FireServer/Resources/OP_8mkM3L34.mp4");
	if(!capture.isOpened())
	{
		std::cout << "Cannot open video source!" << std::endl;
	}
	std::cout << "[FireDetector]Init" << std::endl;
	namedWindow("Test");
}

void FireDetector::start()
{
#if DUAL_THREAD
	if( pthread_create(&captureThread,NULL,FireDetector::captureFrame,(void*)this)!=0) //using myCode
	{
		std::cout << "Fail to create captureThread" << std::endl;
	}
#endif

	if( pthread_create(&runThread,NULL,FireDetector::run,(void*)this)!=0) //using myCode
	{
		std::cout << "Fail to create fireThread" << std::endl;
	}
	std::cout << "[FireDetector]Start" << std::endl;
//	display->startDisplayThread();
}

void FireDetector::join()
{
	pthread_join(runThread, NULL);
#if DUAL_THREAD
	pthread_join(captureThread, NULL);
#endif
}

int FireDetector::getFireThreshold() const
{
	return 20;
}

void* FireDetector::run(void* arg)
{
	FireDetector* obj = (FireDetector*)arg;
	cv::Mat aFrame, tempFrame, sendFrame;
	Mat back;
	Mat front;
	BackgroundSubtractorMOG2 bg;
	unsigned int frameCount = 0;

	while(true)
	{
		if(!obj->capture.read(tempFrame))
		{
			std::cout << "[Run Loop]Cannot read frame" << std::endl;
			obj->capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
			continue;
		}

		imshow("Test", tempFrame);

//		cv::resize(tempFrame, aFrame, cv::Size(640,480), 0, 0, cv::INTER_CUBIC);

//		cv::resize(tempFrame, sendFrame, cv::Size(320,240), 0, 0, cv::INTER_CUBIC);

//		int start_s=clock();
//		int firePixel = obj->getFirePixelNumber(aFrame);
//
//		if(firePixel>obj->getFireThreshold())
//		{
//			frameCount++;
//			if(frameCount>30)
//			{
//				obj->isFire = true;
//				frameCount = 0;
//			}
//		}
//		int stop_s=clock();
//		std::cout << "time(ms): " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
//		imshow(wName, aFrame);

		usleep(DELAY_TIME_MS*1000);
	}
	return NULL;
}

int FireDetector::getFirePixelNumber(Mat aFrame) {
	std::vector<std::vector<cv::Point> > contours;

	Mat YCrCbFrame;
	Mat YChannel, CrChannel, CbChannel;
	Mat Y_Cb, Cr_Cb;
	Mat colorMask;

	//check for input frame
	if(aFrame.empty())
	{
		return -1;
	}
	//---------------detect moving pixel------------//
	//       using BackgroundSubstractMOG2 			//
	//----------------------------------------------//
	bg.operator ()(aFrame, front);
	bg.getBackgroundImage(back);
	//cv::erode(front,front, cv::Mat());
	//cv::dilate(front, front, cv::Mat());
	cv::medianBlur(front, front, 5);
	cv::findContours(front,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );

	for(unsigned int i = 0; i < contours.size(); i++ )
	{
		//approxPolyDP( contours[i], contours_poly[i], 3.0, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		//minEnclosingCircle( contours_poly[i], center[i], radius[i] );
	}

	for(unsigned int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( 255,255,255 );
		//params:	  input   output       contourIdx	color    thickness
		drawContours( front, contours_poly,    i,       color,   CV_FILLED, 8, vector<Vec4i>(), 0, Point() );
	}

	//----detect fire color----//
	//-------------------------------------------------------------------//
	//	pixel = fire color when											 //
	//			valueY > valueCb &&										 //
	//			valueCr > valueCb &&									 //
	//			(valueY > meanY && valueCr > meanCr && valueCb < meanCb) //
	//-------------------------------------------------------------------//

	//get YCrCb channel
	cvtColor(aFrame, YCrCbFrame, CV_BGR2YCrCb);
	vector<Mat> channels(3);
	split(YCrCbFrame, channels);
	YChannel = channels[0];
	CrChannel = channels[1];
	CbChannel = channels[2];

	//calculate mean of 3 channels: => for further use
//	unsigned char Y_mean, Cr_mean, Cb_mean;
//	Y_mean = (unsigned char)mean(YChannel)[0];
//	Cr_mean = (unsigned char)mean(CrChannel)[0];
//	Cb_mean = (unsigned char)mean(CbChannel)[0];

	colorMask = Mat(aFrame.rows, aFrame.cols, CV_8UC1);
	Y_Cb  = Mat(aFrame.rows, aFrame.cols, CV_8UC1);//YChannel minus CbChannel
	Cr_Cb = Mat(aFrame.rows, aFrame.cols, CV_8UC1);//CrChannel minus CbChannel
	subtract(YChannel, CbChannel, Y_Cb); threshold(Y_Cb, Y_Cb, 10, 255, THRESH_BINARY);
	subtract(CrChannel, CbChannel, Cr_Cb);threshold(Cr_Cb, Cr_Cb, 10, 255, THRESH_BINARY);

	//colorMask = front & Y_Cb & Y_Cr
	bitwise_and(front, Y_Cb, colorMask);
	bitwise_and(colorMask, Cr_Cb, colorMask);

	int fireCount = countNonZero(colorMask);

	cvtColor(front, front, CV_GRAY2BGR);
	cvtColor(Y_Cb, Y_Cb, CV_GRAY2BGR);
	cvtColor(Cr_Cb, Cr_Cb, CV_GRAY2BGR);
	cvtColor(colorMask, colorMask, CV_GRAY2BGR);

	char wName[25];
	sprintf(&(wName[0]),"Frames_%s", sourceName.c_str());
	cvShowManyImages(wName, frame.cols, frame.rows, 5, (unsigned char*)frame.data, (unsigned char*)front.data, (unsigned char*)Y_Cb.data, (unsigned char*)Cr_Cb.data, (unsigned char*)colorMask.data);
//	imshow(wName, frame);
	if(fireCount>fireThreshold)
	{
		//count the frame that contains firePixel surpass threshold
		isFire = true;
	}
	else
	{
		isFire = false;
	}
	return fireCount;
}

void FireDetector::cvShowManyImages(std::string title, int s_cols, int s_rows, int nArgs,...)
{
	// img - Used for getting the arguments
	cv::Mat img;

	// [[disp_image]] - the image in which input images are to be copied
	cv::Mat disp_image;

	unsigned int size;
	unsigned int i;
	unsigned int m, n;
	unsigned int x, y;

	// w - Maximum number of images in a row
	// h - Maximum number of images in a column
	int w, h;

	// scale - How much we have to resize the image
	float scale;
	int max;

	// If the number of arguments is lesser than 0 or greater than 12
	// return without displaying
	if(nArgs <= 0)
	{
		std::cout << "Number of arguments too small...." << std::endl;
		return;
	}
	else if(nArgs > 12)
	{
		std::cout << "Number of arguments too large...." << std::endl;
		return;
	}
	// Determine the size of the image,
	// and the number of rows/cols
	// from number of arguments
	else if (nArgs == 1)
	{
		w = h = 1;
		size = SIZE_1;
	}
	else if (nArgs == 2)
	{
		w = 2; h = 1;
		size = SIZE_1;
	}
	else if (nArgs == 3 || nArgs == 4)
	{
		w = 2; h = 2;
		size = SIZE_1;
	}
	else if (nArgs == 5 || nArgs == 6)
	{
		w = 3; h = 2;
		size = SIZE_2;
	}
	else if (nArgs == 7 || nArgs == 8)
	{
		w = 4; h = 2;
		size = SIZE_2;
	}
	else
	{
		w = 4; h = 3;
		size = SIZE_3;
	}

	// Create a new 3 channel image
	disp_image = cv::Mat(cv::Size((w+1)*GAP+size*w, (h+1)*GAP+size*h), CV_8UC3, 3);

	// Used to get the arguments passed
	va_list args;
	va_start(args, nArgs);

	// Loop for nArgs number of arguments
	for (i = 0, m = GAP, n = GAP; i < nArgs; i++, m += (GAP + size))
	{
		// Get the Pointer to the IplImage
		unsigned char* temp = va_arg(args, unsigned char*);

		// Check whether it is NULL or not
		// If it is NULL, release the image, and return
		img = cv::Mat( s_rows, s_cols, CV_8UC3, temp);
		if(img.empty())
		{
			std::cout << "Invalid arguments" << std::endl;
			return;
		}

		// Find the width and height of the image
		x = img.cols;
		y = img.rows;

		// Find whether height or width is greater in order to resize the image
		max = (x > y)? x: y;

		// Find the scaling factor to resize the image
		scale = (float) ( (float) max / size );

		// Used to Align the images
		if( i % w == 0 && m!= GAP)
		{
			m = GAP;
			n+= (GAP + size);
		}

		// Set the image ROI to display the current image
		cv::Rect roi(m, n, (int)(x/scale), (int)( y/scale ));
		cv::Mat image_roi = disp_image(roi);

		// Resize the input image and copy the it to the Single Big Image
		cv::resize(img, img, cv::Size((int)(x/scale), (int)( y/scale )));
		img.copyTo(image_roi);

		// Reset the ROI in order to display the next image
	}

	// Create a new window, and show the Single Big Image
	imshow(title, disp_image);
//	updateWindow("Display window");

	// End the number of arguments
	va_end(args);
}

} /* namespace TestAlgorithm */
