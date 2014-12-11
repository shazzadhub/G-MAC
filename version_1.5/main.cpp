/*
 * File:   main.cpp
 * Author: shazzad
 *
 * Created on November 19, 2013, 5:30 AM
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstdio>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <highgui.h>

#include "skin_color.h"
#include "Dis_handler.h"
#include "red.h"
#include "cvblob.h"

using namespace std;
using namespace cv;
using namespace cvb;

#define w 320
#define h 240
//#define w 640
//#define h 480
//#define w 320


bool red = false;   // // red is used to switch between "skin" && "red"
bool mouse = false;     // // for Mouse activation
bool clk_double = false;    // // for Double Click Activation
bool interactive = false;
bool fpflag = true;

int oldX = 0, oldY = 0;
int movX = 0, movY = 0;

/*
 *
 */
 void keyboard(int code){
    switch(code){
        case 27:    // "Escape" Key
        exit(0);
        break;

        case 114:    // 'r' for switching to Red
        red = !red;
        if(red) cout << "***   Switching to RED object Detection   ***" << endl;
        else    cout << "**   Switching back to Skin Color Detection  **" << endl;
        break;

        case 109:   // 'm' Mouse && Nevigation Activation
        mouse = !mouse;
            if(mouse) cout << "     Mouse Activated" << endl;
            else cout << "      Mouse Deactivated" << endl;
        break;

        case 100:   // 'd' Double Click Activation
        clk_double = !clk_double;
            if(mouse) cout << "     Double Click Activated" << endl;
            else cout << "      Double Click Deactivated" << endl;
        break;

        case 105:   // 'i' for normal mouse nevigation and the generalized one
        interactive = !interactive;
            if(interactive) cout << "      Activated Generalized Mouse" << endl;
            else cout << "      Activated Normal Mouse" << endl;
        break;
    }
 }



int main(int argc, char** argv) {
    Mat mainframes;
    Mat proxy;
    //Mat thresh;
    Mat skinContainer;
    Mat skin_morphClose;
    //Mat last;
    //Mat skin_thresh;
    Mat se_mop = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
    IplConvKernel* se_mop2 = cvCreateStructuringElementEx(5, 5, 3, 3, CV_SHAPE_RECT, NULL);

    //bool red = false;

//    Size s(5, 5);
//    Point p(1, 1);

    CvFont *myFont = new CvFont;
    CvFont *hudFont = new CvFont;
    cvInitFont(myFont, CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 2);
    cvInitFont(hudFont, CV_FONT_HERSHEY_COMPLEX, 0.8, 0.8, 1, 2);


    VideoCapture capture(0);
    if(capture.isOpened() == false){
        cout << "camera not opened" << endl;
        exit(0);
    }
    cout << "camera width = " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << "  ,  "  << "camera hight = " << capture.get(CV_CAP_PROP_FRAME_HEIGHT) <<endl;
    capture.set(CV_CAP_PROP_FRAME_WIDTH, w);      // // setting the camera width to 640 pixel
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, h);     // //
    SkinColor my_skin;
    Dis_handler del;
    Red redtrack;
    //VideoCapture capture("video.avi");



    CvBlobs blobs;
    CvTracks tracks;
    IplImage *labelImg = cvCreateImage(cvSize(w,h), IPL_DEPTH_LABEL, 1);



    namedWindow("Original feed", CV_WINDOW_OPENGL);
    namedWindow("Skin Threshold", CV_WINDOW_OPENGL);
    namedWindow("Rendering with Blobls", CV_WINDOW_OPENGL);
    //namedWindow("Skin_Threshold_Morphology_CLOSE", CV_WINDOW_OPENGL);
//    namedWindow("without morphology_skin_detection", CV_WINDOW_OPENGL);




    while(true || capture.isOpened()) {
        //Mat thresh;
//        Mat skin;
//        Mat last;
// Aquiring the frames from the camera
        //capture >> mainframes;
        capture.read(mainframes);
        mainframes.copyTo(proxy);                   // // Backing up the original frames into proxy // //
                        imshow("Original feed", proxy);

//        morphologyEx(proxy, proxy, MORPH_OPEN, se_mop);
//        morphologyEx(proxy, proxy, MORPH_CLOSE, se_mop);
//.        blur(proxy, proxy, Size(10, 10));

        if(red == true){
            CvSize imgS = proxy.size();
            IplImage *seg = cvCreateImage(imgS, 8, 1);
            seg = redtrack.sampleRed(proxy);
            cvMorphologyEx(seg, seg, NULL, se_mop2, CV_MOP_OPEN, 1);
            destroyWindow("Skin_Threshold_Morphology_CLOSE");
            my_skin.destroyYCbCr();
            cvShowImage("Skin Threshold", seg);
            unsigned int result = cvLabel(seg, labelImg, blobs);
        }

// Preliminary works for minimyzing noise
        //pyrDown(proxy, proxy);
        //blur(proxy, proxy, Size(5, 5));        // // Blurring with gaussian kernel // // (10, 10) for my camera
        //medianBlur(proxy, proxy, 3);

// Work for Detecting the Skin Color && Smoothing
        else{
            skinContainer = my_skin.sampleSkin(proxy);
                morphologyEx(skinContainer, skin_morphClose, MORPH_CLOSE, se_mop, Point(-1, -1), 1);
                            //imshow("Skin_Threshold_Morphology_CLOSE", skin_morphClose);
            IplImage skinContainer_mask = skinContainer;
            IplImage* skinContainer_ipl = &skinContainer_mask;
            cvSmooth(skinContainer_ipl, skinContainer_ipl, CV_MEDIAN, 7, 7);
            cvMorphologyEx(skinContainer_ipl, skinContainer_ipl, NULL, se_mop2, CV_MOP_CLOSE, 1);

                            cvShowImage("Skin Threshold", skinContainer_ipl);
    /*         cvtColor(proxy, proxy, CV_BGR2HSV);     // // Convertin the BGR color spec into HSV. Thought was good but noct good as YCbCr // //
     *         inRange(proxy, Scalar(0, 38, 69), Scalar(28, 178, 220), skin);
     *         //inRange(proxy, Scalar(0, 38, 75), Scalar(28, 180, 250), skin);
     *                         imshow("without morphology_skin_detection", skin);
     *         morphologyEx(skin, thresh, MORPH_CLOSE, se_mop, Point(-1, -1), 1);     // // CLOSE for closing the gaps according the se_mop kernel // //
     *         erode(thresh, last, Mat());             // // erroding the skin_threshol image for removing extra dot noises // //
     *         morphologyEx(last, last, MORPH_OPEN, se_mop);
     *         medianBlur(last, last, 1);
     *                         imshow("skin_threhhold_morphology", last);
     */
            unsigned int result = cvLabel(skinContainer_ipl, labelImg, blobs);
        }


 // Passing to the CvBlobs for creating and marking the blobs ..
        IplImage frame_mask = proxy;                // // Creating IplImage container for work with the CvBlobs library
        IplImage* frame = &frame_mask;              // // because these libraries are not compatible with Mat
        cvFilterByArea(blobs, 600, 1000000);        // // Filtering the blobs frame to frame to reject the blobs not in the range as we consider as noise.
        cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_ANGLE|CV_BLOB_RENDER_COLOR);
        //cvRenderBlob(labelImg, *blobs, frame, frame);
        cvUpdateTracks(blobs, tracks, 200.0, 5);     // // This is to track the blobs's position frame to frame. Every single blob on each frame is being tracked.
        cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX/*|CV_TRACK_RENDER_TO_STD*/);
                        cvShowImage("Rendering with Blobls",frame);




/** Working with CvBlob contures to drag out each blob moments in an image frame. Then we need to calculate the (x1, y1) posision of that
   particular blob-conture. Point p(x1, y1) actually means the pixel position on the 480 / 320 camera frame. Then We need to map the p with
   the screen Resolution (in our case it is 1920 / 1080) we are using on our OP. This p(x, y) is then passed to the Display Handler class for working it out .**/
        if(mouse){
            for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it){
        // calculating the conture pixel into (640 x 480) camera frame
                double moment10 = it->second->m10;
                double moment01 = it->second->m01;
                double area = it->second->area;
                //cout << moment10 << " , " << moment01 << " , " << area << endl;
                int x1 = (int)moment10 / area;
                int y1 = (int)moment01 / area;
                //cout << x1 << " , " << y1 << endl;
        // Mapping to our resolution
                if(x1 > 0 && y1 > 0){
                    int x = (int)(x1 * 1920 / w);   // Mapping the correct width with 1920 for (1920 x 1080) res.
                    int y = (int)(y1 * 1080 / h);   // Mapping with 1080 hight
                    //cout << "X = " << x << "y = " << y << endl;;
                    stringstream mx;
                    mx << "Position : " << "[" << x << " , " << y << "]";
                    cvPutText(frame, mx.str().c_str(), cvPoint(0, 20), hudFont, CV_RGB(255.0, 0.0, 0.0));     // // Printing the coord on frame image
                    //cvRectangle(frame, cvPoint(100, 100), cvPoint(0, 0), CvScalar(0.0, 230.0, 230.0), 1);
                        cvShowImage("Rendering with Blobls",frame);
                    //Point avg = del.doAvarage(x, y, 3);
                    //cout << avg << endl;
                    if(interactive){
                        if(fpflag == true){
                            movX = 1920 / 2;
                            movY = 1080 / 2;
                            oldX = x;
                            oldY = y;
                            fpflag = false;
                        }
                        else{
                            cout << movX << " , " << movY << endl << "      " << x << " , " << y << endl;
                            int dx = x - oldX;
                            int dy = y - oldY;

                            movX += dx;
                            movY += dy;


                            oldX = x; oldY = y;
                        }
                    }
                    else{
                        movX = x;
                        movY = y;
                    }

                    int finger = blobs.size();
                                                                            switch(finger){
                                                                                //case 1: // Mouse Nevigate //
                                                                                //del.mouseMove(x, y);
                                                                                //break;

                                                                                case 1: // Drag
                                                                                del.mouseSingleClick(Button1);
                                                                                //usleep(50);
                                                                                del.mouseMove(movX, movY);
                                                                                break;

//                                                                                case 4: // one single Click
//                                                                                del.mouseClickandRelease(Button1);
//                                                                                del.mouseSingleClick(Button1);
//                                                                                usleep(100);
//                                                                                del.mouseSingleRelease(Button1);
//                                                                                break;

//                                                                                case 2: // Double Click  Left//
//                                                                                del.mouseClickandRelease(Button1);
//                                                                                del.mouseClickandRelease(Button1);
//                                                                                break;

                                                                                if(clk_double){
                                                                                    case 3: // Double Click  Left//
                                                                                    del.mouseSingleClick(Button1);
                                                                                    del.mouseSingleRelease(Button1);
                                                                                    usleep(100);
                                                                                    del.mouseSingleClick(Button1);
                                                                                    del.mouseSingleRelease(Button1);
                                                                                    break;
                                                                                }

                                                                                        // Left Button Click = Button1
                                                                                        // Right Button Click = Button3
                                                                                        // Middle Mouse Button = Button2
                                                                            }
                }
                //del.mouseTo(x, y);
                //cout << "blob has" <<endl;
            }
        }

        //if(waitKey(1) == 27) break;
        int key = waitKey(1);
        keyboard(key);
        cvReleaseBlobs(blobs);
    }


    return EXIT_SUCCESS;
}












//                                                                            switch(finger){
////                                                                                case 1: // Mouse Nevigate //
////                                                                                del.mouseMove(x, y);
////                                                                                break;
//
//                                                                                case 1: // Drag
//                                                                                del.mouseSingleClick(Button1);
//                                                                                //usleep(50);
//                                                                                del.mouseMove(x, y);
//                                                                                break;
//
//                                                                                case 4: // one single Click
//                                                                                //del.mouseClickandRelease(Button1);
//                                                            //                    del.mouseSingleClick(Button1);
//                                                            //                    usleep(100);
//                                                            //                    del.mouseSingleRelease(Button1);
//                                                                                break;
//
//                                                                        //                    case 2: // Double Click  Left//
//                                                                        //                    del.mouseClickandRelease(Button1);
//                                                                        //                    del.mouseClickandRelease(Button1);
//                                                                        //                    break;
//
//                                                                                if(clk_double){
//                                                                                    case 3: // Double Click  Left//
//                                                                                    del.mouseSingleClick(Button1);
//                                                                                    del.mouseSingleRelease(Button1);
//                                                                                    usleep(100);
//                                                                                    del.mouseSingleClick(Button1);
//                                                                                    del.mouseSingleRelease(Button1);
//                                                                                    break;
//                                                                                }
//
//                                                                                        // Left Button Click = Button1
//                                                                                        // Right Button Click = Button3
//                                                                                        // Middle Mouse Button = Button2
//                                                                            }
//                }
//                //del.mouseTo(x, y);
//                //cout << "blob has" <<endl;
//
//            }
//            //cout << "old = "<< oldx << endl;
//        }
//
//        //if(waitKey(1) == 27) break;
//        int key = waitKey(1);
//        keyboard(key);
//        cvReleaseBlobs(blobs);
//    }
//
//
//    return EXIT_SUCCESS;
//}


