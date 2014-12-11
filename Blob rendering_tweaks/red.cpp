#include "red.h"

using namespace cv;

Red::Red(){}

IplImage* Red::sampleRed(Mat in){
    IplImage img_mask = in;
    IplImage *img = &img_mask;
    CvSize imgSize = cvGetSize(img);
    IplImage *frame = cvCreateImage(imgSize, img->depth, img->nChannels);
    cvConvertScale(img, frame, 1, 0);
    IplImage *segmentated = cvCreateImage(imgSize, 8, 1);
    for (unsigned int j=0; j<imgSize.height; j++)
        for (unsigned int i=0; i<imgSize.width; i++)
        {
            CvScalar c = cvGet2D(frame, j, i);
            double b = ((double)c.val[0])/255.;
            double g = ((double)c.val[1])/255.;
            double r = ((double)c.val[2])/255.;
            unsigned char f = 255*((r>0.2+g)&&(r>0.2+b));
            cvSet2D(segmentated, j, i, CV_RGB(f, f, f));
        }
    return segmentated;
}
