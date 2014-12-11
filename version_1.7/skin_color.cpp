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
    namedWindow("Into YCbCr color spec", CV_WINDOW_OPENGL);
}

void SkinColor::destroyYCbCr(){
    destroyWindow("Into YCbCr color spec");
}

Mat SkinColor::sampleSkin(Mat src){
    Mat skin;
    cvtColor(src, skin, COLOR_BGR2YCrCb);
    imshow("Into YCbCr color spec", skin);
    inRange(skin,cv::Scalar(Y_MIN,Cr_MIN,Cb_MIN),cv::Scalar(Y_MAX,Cr_MAX,Cb_MAX),skin);
    return skin;
}

SkinColor::~SkinColor(){
    cout << "Destroying the Skin Detection object" << endl;
}
