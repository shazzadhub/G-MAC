/*
 * File:   Dis_handler.h
 * Author: shazzad
 *
 * Created on November 23, 2013, 11:26 PM
 */

#ifndef DIS_HANDLER_H
#define	DIS_HANDLER_H
#include <iostream>
#include <opencv2/core/core.hpp>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
//#include <X11/Xwindows.h>

using namespace std;
using namespace cv;

class Dis_handler{
    private:
        vector<Point> pa;
    public:
        Dis_handler();
        ~Dis_handler();
        Point doAvarage(int x, int y, int acount);
        void mouseMove(int x, int y);
        void mouseSingleClick(int button);
        void mouseSingleRelease(int button);
        void mouseClickandRelease(int button);

};


#endif	/* DIS_HANDLER_H */
