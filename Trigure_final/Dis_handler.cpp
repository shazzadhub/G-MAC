#include "Dis_handler.h"

using namespace std;

Dis_handler::Dis_handler(){

}

Dis_handler::~Dis_handler(){
    pa.clear();
}



Point Dis_handler::doAvarage(int x, int y, int acount){
    Point c(x, y);
    for(int i = 0; i < acount; i++){
        pa.push_back(c);
    }
    for(int i = 0; i < pa.size(); i++){

    }
    pa.clear();
    return c;
}


void Dis_handler::mouseMove(int x, int y){      // Nevigates the mouse on the Window
    Display *display = XOpenDisplay(0);
    //cout << "X server connected through display" << endl;
    Window root = DefaultRootWindow(display);
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XFlush(display);
    XCloseDisplay(display);
    //cout << "X server disconnected" << endl;
}



void Dis_handler::mouseSingleClick(int button){
	Display *display = XOpenDisplay(NULL);
	XEvent event;
	if(display == NULL){
		cout<<"Failed to Connect with X server."<<endl;
		exit(EXIT_FAILURE);
	}
	memset(&event, 0x00, sizeof(event));
	event.type = ButtonPress;
	event.xbutton.button = button;
	event.xbutton.same_screen = True;

	XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

	event.xbutton.subwindow = event.xbutton.window;
	while(event.xbutton.subwindow){
		event.xbutton.window = event.xbutton.subwindow;
		XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	}
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
        cout<<"Error"<<endl;
	XFlush(display);
	XCloseDisplay(display);
}



void Dis_handler::mouseSingleRelease(int button){
	Display *display = XOpenDisplay(NULL);
	XEvent event;
	if(display == NULL){
		cout<<"Failed to Connect with X server."<<endl;
		exit(EXIT_FAILURE);
	}
	memset(&event, 0x00, sizeof(event));
	event.xbutton.button = button;
	event.xbutton.same_screen = True;
	XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	event.xbutton.subwindow = event.xbutton.window;
	while(event.xbutton.subwindow){
		event.xbutton.window = event.xbutton.subwindow;
		XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	}
	event.type = ButtonRelease;
	event.xbutton.state = 0x100;
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
        cout<<"Error"<<endl;
	XFlush(display);
	XCloseDisplay(display);
}




void Dis_handler::mouseClickandRelease(int button){
    Display *display = XOpenDisplay(NULL);

    XEvent event;

    if(display == NULL){
        cout << "Failed to Connect with X server" << endl;
        exit(EXIT_FAILURE);
    }

    memset(&event, 0x00, sizeof(event));

    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;

    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow){
        event.xbutton.window = event.xbutton.subwindow;

        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
        cout << "Error" << endl;
    XFlush(display);

    //usleep(100);

    event.type = ButtonRelease;
    event.xbutton.state = 0x100;

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
        cout << "Error" << endl;

    XFlush(display);

    XCloseDisplay(display);
}

