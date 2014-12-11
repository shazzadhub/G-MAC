/*
 * File:   Dis_handler.h
 * Author: shazzad
 *
 * Created on November 24, 2013, 02:13 AM
 */

#ifndef RED_H
#define	RED_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class Red{
    public:
        Red();
        IplImage* sampleRed(Mat in);

};



#endif	/* RED_H */
