#include "skin_color.h"

using namespace std;
using namespace cv;


SkinColor::SkinColor(){
    //YCrCb threshold
    Y_MIN  = 0;
    Y_MAX  = 255;
    Cr_MIN = 133;
    Cr_MAX = 173;
    Cb_MIN = 77;
    Cb_MAX = 127;
    showYCbCr();
    //namedWindow("Into YCbCr color spec", CV_WINDOW_OPENGL);
}

void SkinColor::showYCbCr(){
    //namedWindow("Into YCbCr color spec", CV_WINDOW_OPENGL);
}

void SkinColor::destroyYCbCr(){
    destroyWindow("Into YCbCr color spec");
}

Mat SkinColor::sampleSkin(Mat src){
    Mat skin;
    cvtColor(src, skin, COLOR_BGR2YCrCb);
    //imshow("Into YCbCr color spec", skin);
    inRange(skin,cv::Scalar(Y_MIN,Cr_MIN,Cb_MIN),cv::Scalar(Y_MAX,Cr_MAX,Cb_MAX),skin);
   
    IplImage skin_p = skin;
    IplImage *skin_m = &skin_p;
    
    CvSize imgSize = cvGetSize(skin_m);
    IplImage *segmentated = cvCreateImage(imgSize, 8, 1);
    
//    for (unsigned int j=0; j<imgSize.height; j++)
//        for (unsigned int i=0; i<imgSize.width; i++){
//            CvScalar c = cvGet2D(skin_m, j, i);
//            double b = ((double)c.val[0])/255.0;
//            unsigned char f = 230* b;
//                cvSet2D(segmentated, j, i, CV_RGB(f, f, f));
//            //cout << c.val[0] << endl;
//        }
//    cvShowImage("skin_thresh_mod", segmentated);
    return skin;
}

Mat SkinColor::binContours(Mat input){
    Mat input_copy = input.clone();     // // threshold coppied
    Mat skinContour = Mat::zeros(input_copy.size(), CV_8UC3);
    RNG rng(12345);
    
    vector< vector<Point> > contours;   // // contours are stored as vector of points
    vector<Vec4i> hierarchy;
    
    findContours(input_copy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));      // // Contours have detected
    /** void findContours(InputOutputArray image, OutputArrayOfArrays contours, OutputArray hierarchy, int mode, int method, Point offset=Point()) */
    
    for(int i = 0; i < contours.size(); i++){
        //Scalar color(0, 80, 255);
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );        
        drawContours(skinContour, contours, i, color, 2, 8, hierarchy, 0, Point());
        /** void drawContours(InputOutputArray image, InputArrayOfArrays contours, int contourIdx, const Scalar& color, int thickness=1, int lineType=8, InputArray hierarchy=noArray(), int maxLevel=INT_MAX, Point offset=Point() ) **/
    }
    return skinContour;
}

void SkinColor::setDefault(){
    Y_MIN  = 0;
    Y_MAX  = 255;
    Cr_MIN = 133;
    Cr_MAX = 173;
    Cb_MIN = 77;
    Cb_MAX = 127;
    cout << "All value set to be default." << endl;
}

SkinColor::~SkinColor(){
    cout << "Destroying the Skin Detection object" << endl;
}
