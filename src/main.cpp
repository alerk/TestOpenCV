#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <stdarg.h>

using namespace cv;

#define TESTING_ALGORITHM 1

//Prototype
const int unsigned GAP = 10;
const int unsigned SIZE_1 = 360;
const int unsigned SIZE_2 = 320;
const int unsigned SIZE_3 = 280;
const int unsigned DELAY_TIME_MS = 30;

const int fireThreshold = 20;


    cv::Mat back;
    cv::Mat front;
    cv::BackgroundSubtractorMOG2 bg;

void cvShowManyImages(std::string title, int s_cols, int s_rows, int nArgs,...);
int getFirePixelNumber(Mat aFrame);

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

//	TestAlgorithm::FireDetector fire;
    //fire.init();
    //fire.start();
//	fire.join();

    cv::Mat frame;

//    cv::VideoCapture capture = cv::VideoCapture("/home/quyen/work/eclipse/workspace/FireServer/Resources/OP_8mkM3L34.mp4");
    cv::VideoCapture capture = cv::VideoCapture("/home/quyen/Work/Video_files/video.avi");
    if(!capture.isOpened())
    {
        std::cout << "Cannot open video source!" << std::endl;
    }

    while(true)
    {
        if(!capture.read(frame))
        {
            std::cout << "[Run Loop]Cannot read frame" << std::endl;
            capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
            continue;
        }
//		std::cout << "[Run Loop]Show image: "<< frame.cols<< "x"<< frame.rows << std::endl;
//		cv::imshow("Test", frame);
        int firedPixel = getFirePixelNumber(frame);
        char c = cv::waitKey(30);
        if(c=='q')
        {
            break;
        }
    }
    std::cout << "Program end!" << std::endl;
    return 0;
}

int getFirePixelNumber(Mat aFrame) {
    const int ROI_WIDTH = 40;
    const int ROI_HEIGHT = 30;
    unsigned int currentWidth = 0, currentHeight = 0;
    unsigned int width, height;
    std::vector<std::vector<cv::Point> > contours;
    //Mat roi;
    Rect roi;
    width = aFrame.cols;
    height = aFrame.rows;

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
        cv::approxPolyDP( contours[i], contours_poly[i], 3.0, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        cv::minEnclosingCircle( contours_poly[i], center[i], radius[i] );
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

    for(currentWidth = 0; currentWidth < width; currentWidth+=ROI_WIDTH)
    {
        for(currentHeight = 0; currentHeight < height; currentHeight+=ROI_HEIGHT)
        {
            roi = Rect(currentWidth, currentHeight, ROI_WIDTH, ROI_HEIGHT);
            cv::Mat testArea = colorMask(roi);
            int fireCount = countNonZero(testArea);
            if(fireCount > 10)
            {
                cv::Mat roi_draw = aFrame(roi);
                cv::Mat color(roi_draw.size(), CV_8UC3, cv::Scalar (0,125,125));
                double alpha = 0.5;
                cv::addWeighted(color, alpha, roi_draw, 1.0-alpha, 0.0, roi_draw);
            }
        }
    }

    int fireCount = countNonZero(colorMask);

    cvtColor(front, front, CV_GRAY2BGR);
    cvtColor(Y_Cb, Y_Cb, CV_GRAY2BGR);
    cvtColor(Cr_Cb, Cr_Cb, CV_GRAY2BGR);
    cvtColor(colorMask, colorMask, CV_GRAY2BGR);

    char wName[25];
    sprintf(&(wName[0]),"Frames");
    cvShowManyImages(wName, aFrame.cols, aFrame.rows, 5, (unsigned char*)aFrame.data, (unsigned char*)front.data, (unsigned char*)Y_Cb.data, (unsigned char*)Cr_Cb.data, (unsigned char*)colorMask.data);
//	imshow(wName, frame);
//    if(fireCount>fireThreshold)
//    {
//        //count the frame that contains firePixel surpass threshold
//        std::cout << "Fired" << std::endl;
//    }
//    else
//    {
//        std::cout << "Not fired" << std::endl;
//    }
    return fireCount;
}

void cvShowManyImages(std::string title, int s_cols, int s_rows, int nArgs,...)
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


//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/core.hpp"
//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>

//using namespace cv;

//Mat src; Mat src_gray;
//int thresh = 100;
//int max_thresh = 255;
//RNG rng(12345);

///// Function header
//void thresh_callback(int, void* );

///** @function main */
//int main( int argc, char** argv )
//{
//    cv::Mat frame;

//    cv::VideoCapture capture = cv::VideoCapture("/home/quyen/work/eclipse/workspace/FireServer/Resources/OP_8mkM3L34.mp4");
//    if(!capture.isOpened())
//    {
//        std::cout << "Cannot open video source!" << std::endl;
//    }

//    namedWindow("Test");

//    while(true)
//    {
//        /// Load source image and convert it to gray
//        if(!capture.read(src))
//        {
//            std::cout << "[Run Loop]Cannot read frame" << std::endl;
//            capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
//            continue;


//        }



//        /// Convert image to gray and blur it
//        cvtColor( src, src_gray, CV_BGR2GRAY );
//        blur( src_gray, src_gray, Size(3,3) );

//        /// Create Window
//        char* source_window = "Source";
//        namedWindow( source_window, CV_WINDOW_AUTOSIZE );
//        imshow( source_window, src );

//        createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
//        thresh_callback( 0, 0 );

//        char c = waitKey(0);
//        if(c=='q')
//        {
//            break;
//        }
//    }
//    return(0);
//}

///** @function thresh_callback */
//void thresh_callback(int, void* )
//{
//    Mat threshold_output;
//    std::vector<std::vector<cv::Point> > contours;
//    std::vector<Vec4i> hierarchy;

//    /// Detect edges using Threshold
//    threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
//    /// Find contours
//    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

//    /// Approximate contours to polygons + get bounding rects and circles
//    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
//    std::vector<cv::Rect> boundRect( contours.size() );
//    std::vector<cv::Point2f>center( contours.size() );
//    std::vector<float>radius( contours.size() );

//    for( int i = 0; i < contours.size(); i++ )
//    {
//        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
//        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
//        minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
//    }


//    /// Draw polygonal contour + bonding rects + circles
//    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
//    for( int i = 0; i< contours.size(); i++ )
//    {
//        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
//        drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
//        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
//        circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
//    }

//    /// Show in a window
//    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
//    imshow( "Contours", drawing );
//}
