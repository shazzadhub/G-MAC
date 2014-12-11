/*
 * File:   skin_color.h
 * Author: shazzad
 *
 * Created on November 22, 2013, 1:03 PM
 */

#ifndef SKIN_COLOR_H
#define	SKIN_COLOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

class SkinColor {
//    private:
//        int Y_MIN;
//        int Y_MAX;
//        int Cr_MIN;
//        int Cr_MAX;
//        int Cb_MIN;
//        int Cb_MAX;

    public:
        int Y_MIN;
        int Y_MAX;
        int Cr_MIN;
        int Cr_MAX;
        int Cb_MIN;
        int Cb_MAX;

        SkinColor(void);
        ~SkinColor(void);
        void showYCbCr();
        void destroyYCbCr();
        void setDefault();
        Mat binContours(Mat input);
        Mat sampleSkin(Mat src);
};


#endif	/* SKIN_COLOR_H */

