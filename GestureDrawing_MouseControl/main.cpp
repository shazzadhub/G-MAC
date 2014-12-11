/*
 * File:   main.cpp
 * Author: shazzad
 *
 * Created on November 19, 2013, 5:30 AM


    m -> for mouse activation and nevigation
    r -> switching to only red color detection || switching back to skin
    i -> switching to original mouse motion
    d -> enable draging
    c -> enable left clicking (single clicking)
    n -> enable double clicking
    R -> enables right clicking
    esc -> Quit
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <ctime>

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


bool red = false;           // // red is used to switch between "skin" && "red"
bool mouse = false;         // // for Mouse activation
bool drag = false;
bool click = false;
bool clk_double = false;    // // for Double Click Activation
bool interactive = false;   // // for holding the last mouse position based mouse system
bool fpflag = true;         // // for first occurance of interactive
bool application = false;   // // Switch for application mode
bool default_skin_values = false;
bool motiongesture = false;

int oldX = 0, oldY = 0;     // // last position holder
int movX = 0, movY = 0;     // // actual positions that will be passed to Dis_handler

int reinsertcount = 0;
bool frameout = false;
int holdX = 0, holdY = 0;


int t_holder = 0;
vector<Point> currentGest;
string gesin = "Perform Gesture";
string gesout = "Gesture Paused";
string gesav = "Gesture Activated";
string doubleclk = "Double Click Activated";




CvPoint renderPoint;

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

        case 99:
            click = !click;
            if(click) cout << "Clicking Activated" << endl;
            else cout << "      clicking Deactivated" << endl;
        break;

        case 109:   // 'm' Mouse && Nevigation Activation
            mouse = !mouse;
            if(mouse) cout << "Mouse Activated" << endl;
            else cout << "      Mouse Deactivated" << endl;
        break;

        case 110:   // 'n' Double Click Activation
            clk_double = !clk_double;
            if(clk_double) cout << "Double Click Activated" << endl;
            else cout << "      Double Click Deactivated" << endl;
        break;

        case 100:   // 'd' Drag Click Activation
            drag = !drag;
            if(drag) cout << "Drag Activated" << endl;
            else cout << "      Drag Deactivated" << endl;
        break;

        case 105:   // 'i' for normal mouse nevigation and the generalized one
            interactive = !interactive;
            if(interactive) cout << "Activated Generalized Mouse" << endl;
            else cout << "      Activated Normal Mouse" << endl;
        break;

        case 97:    // 'a' for application opener control
            application = !application;
            if(application){
                cout << "###   ##  Switching to Application mode" << endl;
                click = false;
                drag = false;
                interactive = false;
            }
            else cout << "      Switching Back to Normal mode" << endl;
        break;

        case 118: //key = v for resetting skin threshold values to default
            default_skin_values = !default_skin_values;
            if(default_skin_values) cout << "\nResetting to default skin thresholds\nChoose your own skin threshold from the trackbar by pressing the key \"v\"\n" << endl;
            else cout << "Trackbar is activated for you to choose your own skin threshold\n" << endl;
        break;
        
        case 103: //key = g for activationg gesture
            motiongesture = !motiongesture;
            if(motiongesture) cout << "Activated Motion Gesture" << endl;
            else cout << "Deactivated Motion Gesture" << endl;
        break;
    }
 }




int main(int argc, char** argv) {
    Mat mainframes;     /** HOLDS THE MAIN CAPTURE */
    Mat proxy;          /** SAVES A COPY OF MAIN CAPTURE */
    Mat skinContainer;  /** Will HOLD THE NOISELESS SKIN THRESHOLD */
    Mat skinContour;    /** HOLDS THE CONTOURS DRAWN-SKIN THRESHOLD */
    Mat skin_morphClose;

    
    Mat se_mop = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
    Mat se_mop2_1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(3, 3));
    IplConvKernel* se_mop2 = cvCreateStructuringElementEx(5, 5, 3, 3, CV_SHAPE_RECT, NULL);

    
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



    CvFont *myFont = new CvFont;
    CvFont *hudFont = new CvFont;
    cvInitFont(myFont, CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 2);
    cvInitFont(hudFont, CV_FONT_HERSHEY_COMPLEX, 0.8, 0.8, 1, 2);



    CvBlobs blobs;
    CvTracks tracks;
    IplImage *labelImg = cvCreateImage(cvSize(w,h), IPL_DEPTH_LABEL, 1);



    //namedWindow("Original feed", CV_WINDOW_OPENGL);
    //namedWindow("Skin Threshold", CV_WINDOW_OPENGL);
    namedWindow("Skin-Conture Threshold", CV_WINDOW_OPENGL);
    namedWindow("Rendering with Blobls", CV_GUI_EXPANDED);
    cvLoadWindowParameters("Rendering with Blobls");
    moveWindow("Rendering with Blobls", 1600, 1000);
    //cvResizeWindow("Rendering with Blobls", 1024, 786);


    createTrackbar("Ymin", "Original feed", &my_skin.Y_MIN, 256, 0);
   

    while(true || capture.isOpened()) {
// Aquiring the frames from the camera
        capture >> mainframes;
        mainframes.copyTo(proxy);                               // // Backing up the original frames into proxy // //
                        //moveWindow("Original feed", 0, 750);
                        //imshow("Original feed", proxy);        

        if(red == true){                                        // // Red-Object Detection Mode
            //blur(proxy, proxy, Size(10, 10));
            CvSize imgS = proxy.size();
            IplImage *seg = cvCreateImage(imgS, 8, 1);
            seg = redtrack.sampleRed(proxy);
            cvMorphologyEx(seg, seg, NULL, se_mop2, CV_MOP_OPEN, 1);
            destroyWindow("Skin_Threshold_Morphology_CLOSE");
            my_skin.destroyYCbCr();
            //cvShowImage("Skin Threshold", seg);
            unsigned int result = cvLabel(seg, labelImg, blobs);
        }

// Preliminary works for minimyzing noise
        //pyrDown(proxy, proxy);
        //blur(proxy, proxy, Size(5, 5));                       // // Blurring with gaussian kernel // // (10, 10) for my camera
        //medianBlur(proxy, proxy, 3);

// Work for Detecting the Skin Color && Smoothing
        else{
            if(default_skin_values) my_skin.setDefault();
            skinContainer = my_skin.sampleSkin(proxy);
                //morphologyEx(skinContainer, skin_morphClose, MORPH_CLOSE, se_mop, Point(-1, -1), 1);
                            //imshow("Skin_Threshold_Morphology_CLOSE", skin_morphClose);
            medianBlur(skinContainer, skinContainer, 7);
            morphologyEx(skinContainer, skinContainer, MORPH_CLOSE, se_mop2_1, Point(-1, -1), 1);
            
            skinContour = my_skin.binContours(skinContainer);
                            moveWindow("Skin-Conture Threshold", 1600, 300);    // // Fixes the window x,y position
                            imshow("Skin-Conture Threshold", skinContour);
            
            IplImage skinContainer_mask = skinContainer;
            IplImage* skinContainer_ipl = &skinContainer_mask;
            
//            cvSmooth(skinContainer_ipl, skinContainer_ipl, CV_MEDIAN, 7, 7);
//            cvMorphologyEx(skinContainer_ipl, skinContainer_ipl, NULL, se_mop2, CV_MOP_CLOSE, 1);    // // C-Api 

                            //cvShowImage("Skin Threshold", skinContainer_ipl);                          // ------------>> Noiseless Skin-threshold 
                            
            unsigned int result = cvLabel(skinContainer_ipl, labelImg, blobs);
        }


 // Passing to the CvBlobs for creating and marking the blobs ..
        IplImage frame_mask = proxy;                // // Creating IplImage container for work with the CvBlobs library
        IplImage* blobFrame = &frame_mask;              // // because these libraries are not compatible with Mat
        cvFilterByArea(blobs, 800, 1000000);        // // Filtering the blobs frame to frame to reject the blobs not in the range as we consider as noise.
        cvRenderBlobs(labelImg, blobs, blobFrame, blobFrame, CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_ANGLE|CV_BLOB_RENDER_COLOR);
        cvUpdateTracks(blobs, tracks, 200.0, 5);     // // This is to track the blobs's position frame to frame. Every single blob on each frame is being tracked.
        cvRenderTracks(tracks, blobFrame, blobFrame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX/*|CV_TRACK_RENDER_TO_STD*/);
        
        for(CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it){

          CvContourPolygon *polygon = cvConvertChainCodesToPolygon(&(*it).second->contour);

          CvContourPolygon *sPolygon = cvSimplifyPolygon(polygon, 5.0);
          CvContourPolygon *cPolygon = cvPolygonContourConvexHull(sPolygon);

          cvRenderContourChainCode(&(*it).second->contour, blobFrame, CV_RGB(255, 0 ,0));
          cvRenderContourPolygon(sPolygon, blobFrame, CV_RGB(0, 0, 255));
          cvRenderContourPolygon(cPolygon, blobFrame, CV_RGB(0, 255, 0));

          delete cPolygon;
          delete sPolygon;
          delete polygon;

          // Render internal contours:
          //for(CvContoursChainCode::const_iterator jt=(*it).second->internalContours.begin(); jt!=(*it).second->internalContours.end(); ++jt)
            //cvRenderContourChainCode((*jt), blobFrame);
          
          renderPoint = getshPoint();
          renderPoint.x - 2.0;
          renderPoint.y - 2.0;
          //cvCircle(blobFrame, renderPoint, 3, CV_RGB(0.0, 255.0, 255.0), 1);
        }
        
                            cvCircle(blobFrame, renderPoint, 3, CV_RGB(0.0, 255.0, 255.0), 4, CV_AA, 0);
                            cvShowImage("Rendering with Blobls",blobFrame);





// **********************************************************************   MOUSE  ***************************************************************************** //
/** Working with CvBlob contures to drag out each blob moments in an image frame. Then we need to calculate the (x1, y1) posision of that
   particular blob-conture. Point p(x1, y1) actually means the pixel position on the 480 / 320 camera frame. Then We need to map the p with
   the screen Resolution (in our case it is 1920 / 1080) we are using on our OP. This p(x, y) is then passed to the Display Handler class for working it out .**/
        if(mouse){
            if(blobs.empty()){                       // // if the hands gets out of the camera frame
                //cout << "No blobs .." <<endl;
                frameout = true;                // // turning the FRAMEOUT flag to true to indicate hand is out of frame
                t_holder = 0;                   // // IF hand is out the time clock will reset to zero
                currentGest.clear();            // // clears the gesture buffer
            }
                       
            
            for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it){
        // calculating the conture pixel into (640 x 480) camera frame
                double moment10 = it->second->m10;
                double moment01 = it->second->m01;
                double area = it->second->area;

                int x1 = (int)moment10 / area;
                int y1 = (int)moment01 / area;
                
                
//.........................................................GESTURE RELATED.................................................................//                
                
                
                if(motiongesture){
                        //cout << "Motion Gesture Activated" <<endl;                    
                    time_t t_current = time(NULL);              // // get the current sec from 1/1/1970
                    if(t_holder == 0){
                        t_holder = (int)t_current;
                    }
                    
                    int currentsec = (int)t_current - t_holder;         // // holds time interval sinces hand is on frame
                    
                    if(currentsec <= 4){        // //Time window to perform gesture is 4 sceonds
                        //Point p(x1, y1);
                        //cout << p;
                        currentGest.push_back(Point (x1, y1));
                    }
                    //cout << endl;                    
                    //cout << currentsec << endl;                   
                    
                    else if(currentsec <= 8){
                         Mat currgest = Mat::zeros(640, 480, CV_8UC3);
                         namedWindow("current Gesture", CV_WINDOW_AUTOSIZE);
                         
                         for(int g = 0; g < currentGest.size(); g++){
                             if(g != currentGest.size() - 1)
                                line(currgest, currentGest.at(g), currentGest.at(g + 1), CV_RGB(255.0, 255.0, 255.0), CV_AA);
                             
                             cout << currentGest.at(g) << endl;
                         }
                         imshow("current Gesture", currgest);
                    }
                    
//                    else if(currentsec <= 12){
//                        destroyWindow("current Gesture");
//                        
//                        
//                    }
                    
                    
                }
                else{
                    t_holder = 0;
                    currentGest.clear();
                    destroyWindow("current Gesture");
                }
                
                
 //.........................................................GESTURE RELATED.................................................................//                
                
                
                
                

        // Mapping to our resolution
                if(x1 > 0 && y1 > 0){
                    int x = (int)(x1 * 1920 / w);   // Mapping the correct width with 1920 for (1920 x 1080) res.
                    int y = (int)(y1 * 1080 / h);   // Mapping with 1080 hight

                    holdX = x;
                    holdY = y;
                    stringstream mx;
                    mx << "Position : " << "[" << x << " , " << y << "]";
                    cvPutText(blobFrame, mx.str().c_str(), cvPoint(0, 20), hudFont, CV_RGB(255.0, 0.0, 0.0));     // // Printing the coord on frame image
                    //cvRectangle(blobFrame, cvPoint(100, 100), cvPoint(0, 0), cvScalar(0.0, 230.0, 230.0, 0.0), CV_FILLED);
                        
//                        skinContour = my_skin.contours(blobFrame);
//                        IplImage skCon = skinContour;
//                        IplImage* skinCon = &skCon;                        
                        cvShowImage("Rendering with Blobls",blobFrame);                    
                    //Point avg = del.doAvarage(x, y, 3);
                    //cout << avg << endl;
                    if(interactive){
                        if(fpflag){
                            movX = 1920 / 2;        // // for first occurance fixing the mouse in the middle
                            movY = 1080 / 2;
                            oldX = x;
                            oldY = y;
                            fpflag = false;
                            frameout = false;
                        }
                        else{
                            if(frameout){       // // if hand gets out of the frame
                                oldX = x;
                                oldY = y;
                                reinsertcount++;
                                cout << "Re-inserted .." << reinsertcount << endl;
                                frameout = false;                                                                
                            }
                            else{
                                int dx = x - oldX;      // // calculating difference between last and current position
                                int dy = y - oldY;

                                movX += dx;         // // updating the mouse from freezed position
                                movY += dy;
                                //cout << movX << " , " << movY << endl;
                                if((movX <= 0) || (movY <= 0)){
                                    movX = 0;
                                    movY = 0;
                                }

                                oldX = x; oldY = y;
                            }
                        }
                    }
                    else{       // // Normal (old) mouse nevigations
                        movX = x;
                        movY = y;
                    }

                    int finger = blobs.size();

                        stringstream blobnumPrint;      // // For printing the blob count on statusbar
                        blobnumPrint << "current blob count = " << finger;                        
                        cvDisplayStatusBar("Rendering with Blobls", blobnumPrint.str().c_str(), 1000);
                        if(motiongesture)
                            cvDisplayStatusBar("Rendering with Blobls", gesav.c_str(), 100);
                        if(clk_double)    
                            cvDisplayStatusBar("Rendering with Blobls", doubleclk.c_str(), 3000);                            
                                
                        
                                
                        
                                switch(finger){
                                    case 1:
                                        if(drag){
                                            del.mouseSingleClick(Button1);
                                            del.mouseMove(movX, movY);
                                        }
                                        else if(click){
                                            del.mouseClickandRelease(Button1);
                                        }
                                        else if(clk_double){
                                            del.mouseClickandRelease(Button1);
                                            usleep(10);
                                            del.mouseClickandRelease(Button1);
                                        }
                                        else del.mouseMove(movX, movY);
                                    break;

                                    case 2:
                                        if(application){
                                            cout << " ### Opening Pencil ###" << endl;
                                            system("pencil");
                                            application = !application;
                                        }
                                    break;

                                }
//                                if(finger == 1){
//                                    del.mouseMove(movX, movY);
//                                }
//
//                                else if(finger == 1 && drag){
//                                    del.mouseSingleClick(Button1);
//                                    del.mouseMove(movX, movY);
//                                }

//                                else if(finger == 1 && click){
//                                    del.mouseClickandRelease(Button1);
//                                }
//                                else if(finger == 2 && application){
//                                    cout << " ### Opening Pencil ###" << endl;
//                                    system("pencil");
//                                    application = !application;
//                                }

//                                switch(finger){
//                                    //case 1: // Mouse Nevigate //
//                                    //del.mouseMove(x, y);
//                                    //break;
//
//                                    case 1: // Drag
//                                    del.mouseSingleClick(Button1);
//                                    //usleep(50);
//                                    del.mouseMove(movX, movY);
//                                    break;
//
//                                    case 4: // one single Click
//                                    del.mouseClickandRelease(Button1);
//                                    del.mouseSingleClick(Button1);
//                                    usleep(100);
//                                    del.mouseSingleRelease(Button1);
//                                    break;
//
//                                    case 2: // Double Click  Left//
//                                    del.mouseClickandRelease(Button1);
//                                    del.mouseClickandRelease(Button1);
//                                    break;
//
//                                    if(clk_double){
//                                        case 3: // Double Click  Left//
//                                        del.mouseSingleClick(Button1);
//                                        del.mouseSingleRelease(Button1);
//                                        usleep(100);
//                                        del.mouseSingleClick(Button1);
//                                        del.mouseSingleRelease(Button1);
//                                        break;
//                                    }
//
//                                            // Left Button Click = Button1
//                                            // Right Button Click = Button3
//                                            // Middle Mouse Button = Button2
//                                }
                }
                //del.mouseTo(x, y);
                //cout << "blob has" <<endl;
            } // Blob Detection FOR braket

            //cout << holdX << " , " << holdY << endl;
        } // mouse switch braket

        //if(waitKey(1) == 27) break;
        int key = waitKey(1);
        keyboard(key);
        cvReleaseBlobs(blobs);
        cvReleaseStructuringElement(&se_mop2);
    }    
    return EXIT_SUCCESS;
}
















































/**  ***   Mid-Presentation. Nicely Woking   ***  */


//#include <cstdlib>
//#include <iostream>
//#include <sstream>
//#include <cstdio>
//
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
////#include <highgui.h>
//
//#include "skin_color.h"
//#include "Dis_handler.h"
//#include "red.h"
//#include "cvblob.h"
//
//using namespace std;
//using namespace cv;
//using namespace cvb;
//
//#define w 320
//#define h 240
////#define w 640
////#define h 480
////#define w 320
//
//
//bool red = false;   // // red is used to switch between "skin" && "red"
//bool mouse = false;     // // for Mouse activation
//bool drag = false;
//bool click = false;
//bool clk_double = false;    // // for Double Click Activation
//bool interactive = false;   // // for holding the last mouse position based mouse system
//bool fpflag = true;         // // for first occurance of interactive
//bool application = false;   // // Switch for application mode
//bool default_skin_values = false;
//
//int oldX = 0, oldY = 0;     // // last position holder
//int movX = 0, movY = 0;     // // actual positions that will be passed to Dis_handler
//
//int reinsertcount = 0;
//bool frameout = false;
//int holdX = 0, holdY = 0;
//
///*
// *
// */
// void keyboard(int code){
//    switch(code){
//        case 27:    // "Escape" Key
//            exit(0);
//        break;
//
//        case 114:    // 'r' for switching to Red
//            red = !red;
//            if(red) cout << "***   Switching to RED object Detection   ***" << endl;
//            else    cout << "**   Switching back to Skin Color Detection  **" << endl;
//        break;
//
//        case 99:
//            click = !click;
//            if(click) cout << "Clicking Activated" << endl;
//            else cout << "      clicking Deactivated" << endl;
//        break;
//
//        case 109:   // 'm' Mouse && Nevigation Activation
//            mouse = !mouse;
//            if(mouse) cout << "Mouse Activated" << endl;
//            else cout << "      Mouse Deactivated" << endl;
//        break;
//
//        case 110:   // 'n' Double Click Activation
//            clk_double = !clk_double;
//            if(clk_double) cout << "Double Click Activated" << endl;
//            else cout << "      Double Click Deactivated" << endl;
//        break;
//
//        case 100:   // 'd' Drag Click Activation
//            drag = !drag;
//            if(drag) cout << "Drag Activated" << endl;
//            else cout << "      Drag Deactivated" << endl;
//        break;
//
//        case 105:   // 'i' for normal mouse nevigation and the generalized one
//            interactive = !interactive;
//            if(interactive) cout << "Activated Generalized Mouse" << endl;
//            else cout << "      Activated Normal Mouse" << endl;
//        break;
//
//        case 97:    // 'a' for application opener control
//            application = !application;
//            if(application){
//                cout << "###   ##  Switching to Application mode" << endl;
//                click = false;
//                drag = false;
//                interactive = false;
//            }
//            else cout << "      Switching Back to Normal mode" << endl;
//        break;
//
//        case 118: //key = v for resetting skin threshold values to default
//            default_skin_values = !default_skin_values;
//            if(default_skin_values) cout << "\nResetting to default skin thresholds\nChoose your own skin threshold from the trackbar by pressing the key \"v\"\n" << endl;
//            else cout << "Trackbar is activated for you to choose your own skin threshold\n" << endl;
//        break;
//    }
// }
//
//
//
//int main(int argc, char** argv) {
//    Mat mainframes;
//    Mat proxy;
//    Mat skinContainer;
//    Mat skin_morphClose;
//    //Mat last;
//    //Mat skin_thresh;
//    //Mat thresh;
//    Mat se_mop = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
//    IplConvKernel* se_mop2 = cvCreateStructuringElementEx(5, 5, 3, 3, CV_SHAPE_RECT, NULL);
//
//    //bool red = false;
////    Size s(5, 5);
////    Point p(1, 1);
//
//
//    VideoCapture capture(0);
//    if(capture.isOpened() == false){
//        cout << "camera not opened" << endl;
//        exit(0);
//    }
//    cout << "camera width = " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << "  ,  "  << "camera hight = " << capture.get(CV_CAP_PROP_FRAME_HEIGHT) <<endl;
//    capture.set(CV_CAP_PROP_FRAME_WIDTH, w);      // // setting the camera width to 640 pixel
//    capture.set(CV_CAP_PROP_FRAME_HEIGHT, h);     // //
//    SkinColor my_skin;
//    Dis_handler del;
//    Red redtrack;
//    //VideoCapture capture("video.avi");
//
//
//
//    CvFont *myFont = new CvFont;
//    CvFont *hudFont = new CvFont;
//    cvInitFont(myFont, CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 2);
//    cvInitFont(hudFont, CV_FONT_HERSHEY_COMPLEX, 0.8, 0.8, 1, 2);
//
//
//
//    CvBlobs blobs;
//    CvTracks tracks;
//    IplImage *labelImg = cvCreateImage(cvSize(w,h), IPL_DEPTH_LABEL, 1);
//
//
//
//    namedWindow("Original feed", CV_WINDOW_OPENGL);
//    namedWindow("Skin Threshold", CV_WINDOW_OPENGL);
//    namedWindow("Rendering with Blobls", CV_GUI_EXPANDED);
//    cvLoadWindowParameters("Rendering with Blobls");
//    //namedWindow("Skin_Threshold_Morphology_CLOSE", CV_WINDOW_OPENGL);
//    //namedWindow("without morphology_skin_detection", CV_WINDOW_OPENGL);
//    //cvResizeWindow("Rendering with Blobls", 1024, 786);
//
//
//    createTrackbar("Ymin", "Original feed", &my_skin.Y_MIN, 256, 0);
//
//
//
//    while(true || capture.isOpened()) {
//        //Mat thresh;
////        Mat skin;
////        Mat last;
//// Aquiring the frames from the camera
//        capture >> mainframes;
//        //capture.read(mainframes);
//        mainframes.copyTo(proxy);                   // // Backing up the original frames into proxy // //
//                        imshow("Original feed", proxy);
//
////        morphologyEx(proxy, proxy, MORPH_OPEN, se_mop);
////        morphologyEx(proxy, proxy, MORPH_CLOSE, se_mop);
////.        blur(proxy, proxy, Size(10, 10));
//
//        if(red == true){
//            CvSize imgS = proxy.size();
//            IplImage *seg = cvCreateImage(imgS, 8, 1);
//            seg = redtrack.sampleRed(proxy);
//            cvMorphologyEx(seg, seg, NULL, se_mop2, CV_MOP_OPEN, 1);
//            destroyWindow("Skin_Threshold_Morphology_CLOSE");
//            my_skin.destroyYCbCr();
//            cvShowImage("Skin Threshold", seg);
//            unsigned int result = cvLabel(seg, labelImg, blobs);
//        }
//
//// Preliminary works for minimyzing noise
//        //pyrDown(proxy, proxy);
//        //blur(proxy, proxy, Size(5, 5));        // // Blurring with gaussian kernel // // (10, 10) for my camera
//        //medianBlur(proxy, proxy, 3);
//
//// Work for Detecting the Skin Color && Smoothing
//        else{
//            if(default_skin_values) my_skin.setDefault();
//            skinContainer = my_skin.sampleSkin(proxy);
//                //morphologyEx(skinContainer, skin_morphClose, MORPH_CLOSE, se_mop, Point(-1, -1), 1);
//                            //imshow("Skin_Threshold_Morphology_CLOSE", skin_morphClose);
//            IplImage skinContainer_mask = skinContainer;
//            IplImage* skinContainer_ipl = &skinContainer_mask;
//            cvSmooth(skinContainer_ipl, skinContainer_ipl, CV_MEDIAN, 7, 7);
//            cvMorphologyEx(skinContainer_ipl, skinContainer_ipl, NULL, se_mop2, CV_MOP_CLOSE, 1);
//
//                            cvShowImage("Skin Threshold", skinContainer_ipl);
//    /*         cvtColor(proxy, proxy, CV_BGR2HSV);     // // Convertin the BGR color spec into HSV. Thought was good but not good as YCbCr // //
//     *         inRange(proxy, Scalar(0, 38, 69), Scalar(28, 178, 220), skin);
//     *         //inRange(proxy, Scalar(0, 38, 75), Scalar(28, 180, 250), skin);
//     *                         imshow("without morphology_skin_detection", skin);
//     *         morphologyEx(skin, thresh, MORPH_CLOSE, se_mop, Point(-1, -1), 1);     // // CLOSE for closing the gaps according the se_mop kernel // //
//     *         erode(thresh, last, Mat());             // // erroding the skin_threshol image for removing extra dot noises // //
//     *         morphologyEx(last, last, MORPH_OPEN, se_mop);
//     *         medianBlur(last, last, 1);
//     *                         imshow("skin_threhhold_morphology", last);
//     */
//            unsigned int result = cvLabel(skinContainer_ipl, labelImg, blobs);
//            //cout << result << endl;
//        }
//
//
// // Passing to the CvBlobs for creating and marking the blobs ..
//        IplImage frame_mask = proxy;                // // Creating IplImage container for work with the CvBlobs library
//        IplImage* frame = &frame_mask;              // // because these libraries are not compatible with Mat
//        cvFilterByArea(blobs, 800, 1000000);        // // Filtering the blobs frame to frame to reject the blobs not in the range as we consider as noise.
//        cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_ANGLE|CV_BLOB_RENDER_COLOR);
//        //cvRenderBlob(labelImg, *blobs, frame, frame);
//        cvUpdateTracks(blobs, tracks, 200.0, 5);     // // This is to track the blobs's position frame to frame. Every single blob on each frame is being tracked.
//        cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX/*|CV_TRACK_RENDER_TO_STD*/);
//                        cvShowImage("Rendering with Blobls",frame);
//
//
//
//
///** Working with CvBlob contures to drag out each blob moments in an image frame. Then we need to calculate the (x1, y1) posision of that
//   particular blob-conture. Point p(x1, y1) actually means the pixel position on the 480 / 320 camera frame. Then We need to map the p with
//   the screen Resolution (in our case it is 1920 / 1080) we are using on our OP. This p(x, y) is then passed to the Display Handler class for working it out .**/
//        if(mouse){
//            if(blobs.empty()){      // // if the hands gets out of the camera frame
//                //cout << "No blobs .." <<endl;
//                frameout = true;
//            }
//            for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it){
//        // calculating the conture pixel into (640 x 480) camera frame
//                double moment10 = it->second->m10;
//                double moment01 = it->second->m01;
//                double area = it->second->area;
//                //cout << moment10 << " , " << moment01 << " , " << area << endl;
//                int x1 = (int)moment10 / area;
//                int y1 = (int)moment01 / area;
//                //cout << x1 << " , " << y1 << endl;
//        // Mapping to our resolution
//                if(x1 > 0 && y1 > 0){
//                    int x = (int)(x1 * 1920 / w);   // Mapping the correct width with 1920 for (1920 x 1080) res.
//                    int y = (int)(y1 * 1080 / h);   // Mapping with 1080 hight
//                    //cout << "X = " << x << "y = " << y << endl;;
//                    holdX = x;
//                    holdY = y;
//                    stringstream mx;
//                    mx << "Position : " << "[" << x << " , " << y << "]";
//                    //cvPutText(frame, mx.str().c_str(), cvPoint(0, 20), hudFont, CV_RGB(255.0, 0.0, 0.0));     // // Printing the coord on frame image
//                    //cvRectangle(frame, cvPoint(100, 100), cvPoint(0, 0), CvScalar(0.0, 230.0, 230.0), 1);
//                        cvShowImage("Rendering with Blobls",frame);
//                    //Point avg = del.doAvarage(x, y, 3);
//                    //cout << avg << endl;
//                    if(interactive){
//                        if(fpflag){
//                            movX = 1920 / 2;        // // for first occurance fixing the mouse in the middle
//                            movY = 1080 / 2;
//                            oldX = x;
//                            oldY = y;
//                            fpflag = false;
//                            frameout = false;
//                        }
//                        else{
//                            if(frameout){       // // if hand gets out of the frame
//                                oldX = x;
//                                oldY = y;
//                                reinsertcount++;
//                                cout << "Re-inserted .." << reinsertcount << endl;
//                                frameout = false;
//                            }
//                            else{
//                                int dx = x - oldX;      // // calculating difference between last and current position
//                                int dy = y - oldY;
//
//                                movX += dx;         // // updating the mouse from freezed position
//                                movY += dy;
//                                //cout << movX << " , " << movY << endl;
//                                if((movX <= 0) || (movY <= 0)){
//                                    movX = 0;
//                                    movY = 0;
//                                }
//
//                                oldX = x; oldY = y;
//                            }
//                        }
//                    }
//                    else{       // // Normal (old) mouse nevigations
//                        movX = x;
//                        movY = y;
//                    }
//
//                    int finger = blobs.size();
//
//                        stringstream blobnumPrint;      // // For printing the blob count on statusbar
//                        blobnumPrint << "current blob count = " << finger;
//                        cvDisplayStatusBar("Rendering with Blobls", blobnumPrint.str().c_str(), 1000);
//
//                                switch(finger){
//                                    case 1:
//                                        if(drag){
//                                            del.mouseSingleClick(Button1);
//                                            del.mouseMove(movX, movY);
//                                        }
//                                        else if(click){
//                                            del.mouseClickandRelease(Button1);
//                                        }
//                                        else if(clk_double){
//                                            del.mouseClickandRelease(Button1);
//                                            usleep(10);
//                                            del.mouseClickandRelease(Button1);
//                                        }
//                                        else del.mouseMove(movX, movY);
//                                    break;
//
//                                    case 2:
//                                        if(application){
//                                            cout << " ### Opening Pencil ###" << endl;
//                                            system("pencil");
//                                            application = !application;
//                                        }
//                                    break;
//
//                                }
////                                if(finger == 1){
////                                    del.mouseMove(movX, movY);
////                                }
////
////                                else if(finger == 1 && drag){
////                                    del.mouseSingleClick(Button1);
////                                    del.mouseMove(movX, movY);
////                                }
//
////                                else if(finger == 1 && click){
////                                    del.mouseClickandRelease(Button1);
////                                }
////                                else if(finger == 2 && application){
////                                    cout << " ### Opening Pencil ###" << endl;
////                                    system("pencil");
////                                    application = !application;
////                                }
//
////                                switch(finger){
////                                    //case 1: // Mouse Nevigate //
////                                    //del.mouseMove(x, y);
////                                    //break;
////
////                                    case 1: // Drag
////                                    del.mouseSingleClick(Button1);
////                                    //usleep(50);
////                                    del.mouseMove(movX, movY);
////                                    break;
////
////                                    case 4: // one single Click
////                                    del.mouseClickandRelease(Button1);
////                                    del.mouseSingleClick(Button1);
////                                    usleep(100);
////                                    del.mouseSingleRelease(Button1);
////                                    break;
////
////                                    case 2: // Double Click  Left//
////                                    del.mouseClickandRelease(Button1);
////                                    del.mouseClickandRelease(Button1);
////                                    break;
////
////                                    if(clk_double){
////                                        case 3: // Double Click  Left//
////                                        del.mouseSingleClick(Button1);
////                                        del.mouseSingleRelease(Button1);
////                                        usleep(100);
////                                        del.mouseSingleClick(Button1);
////                                        del.mouseSingleRelease(Button1);
////                                        break;
////                                    }
////
////                                            // Left Button Click = Button1
////                                            // Right Button Click = Button3
////                                            // Middle Mouse Button = Button2
////                                }
//                }
//                //del.mouseTo(x, y);
//                //cout << "blob has" <<endl;
//            }
//            //cout << holdX << " , " << holdY << endl;
//        } // mouse switch parenthesis
//
//        //if(waitKey(1) == 27) break;
//        int key = waitKey(1);
//        keyboard(key);
//        cvReleaseBlobs(blobs);
//    }
//    return EXIT_SUCCESS;
//}






































/** This was written when the first Mouse operations are being developed .. **/
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


