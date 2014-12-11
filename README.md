G-MAC
=====

Gesture-Mouse-Application Controller

This is a "gesture" based application written in C++ with OpenCV, to control the typical mouse operations. The application can detect skin color and based on that it basically creates bobs (bounding rectangle). Then it calculates the centroid and this center point is the key co-ordinate to re-mapped to wanted screen resolution for mouse triggering on Linux based OSs. I used CvBlob libraries for creating blobs and used CvInrange method for skin-tone detection with Asian theme. Mouse operations have two modes. Interactive and normal. Interactive means, if the hand is out of the camera frame the mouse cursor freezes on that location and re-activates from that position when the hand is back. By-the way one simple webcam, OpenCV installation is needed and to run the code. Can be run from both CodeBlocks and Netneans but need to configure them first tough. This tiny program has a lot drawbacks. Low light, non-smooth and "skinny" background is not good for it. Need a lot on that. Wish me luck. Cheers.

Useg
====

Total controlling the mouse and all of it's functions, such as grab, drag, left clicking, right clicking with moving hand in front of the webcam.

Drawing image with hands infront of the webcam in known image editors... gimp or MSPaint.

Playing several games with hand gesture, like fruit ninja.

I created the Generalized mouse nevigation system.. made a switch between normal and that generalized one ... that is 'i'


Instructions for Netbeans
=========================

Additional Library Dir :
/usr/local/include;/usr/local/include/opencv;/usr/local/include/opencv2;/usr/include

Libraries -> Add pkg-config Library, then select -> `pkg-config --libs opencv`

Additional Option -> `lX11`, `lXtst`


Installing OpenCV
=================

```
arch=$(uname -m)
if [ "$arch" == "i686" -o "$arch" == "i386" -o "$arch" == "i486" -o "$arch" == "i586" ]; then
flag=1
else
flag=0
fi
echo "Installing OpenCV 2.4.3"
mkdir OpenCV
cd OpenCV
echo "Removing any pre-installed ffmpeg and x264"
sudo apt-get -y remove ffmpeg x264 libx264-dev
echo "Installing Dependenices"
sudo apt-get -y install libopencv-dev
sudo apt-get -y install build-essential checkinstall cmake pkg-config yasm
sudo apt-get -y install libtiff4-dev libjpeg-dev libjasper-dev
sudo apt-get -y install libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev libxine-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libv4l-dev
sudo apt-get -y install python-dev python-numpy
sudo apt-get -y install libtbb-dev
sudo apt-get -y install libqt4-dev libgtk2.0-dev
sudo apt-get install libfaac-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev
echo "Downloading x264"
wget ftp://ftp.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-20130926-2245-stable.tar.bz2
tar -xvf x264-snapshot-20121114-2245-stable.tar.bz2
cd x264-snapshot-20130926-2245-stable/
echo "Installing x264"
if [ $flag -eq 1 ]; then
./configure --enable-static
else
./configure --enable-shared --enable-pic
fi
make
sudo make install
cd ..
echo "Downloading ffmpeg"
wget http://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2
echo "Installing ffmpeg"
tar -xvf ffmpeg-snapshot.tar.bz2
cd ffmpeg/
if [ $flag -eq 1 ]; then
./configure --enable-gpl --enable-libfaac --enable-libmp3lame --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libtheora --enable-libvorbis --enable-libx264 --enable-libxvid --enable-nonfree --enable-postproc --enable-version3 --enable-x11grab
else
./configure --enable-gpl --enable-libfaac --enable-libmp3lame --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libtheora --enable-libvorbis --enable-libx264 --enable-libxvid --enable-nonfree --enable-postproc --enable-version3 --enable-x11grab --enable-shared
fi
make
sudo make install
cd ..
echo "Downloading v4l"
#wget http://www.linuxtv.org/downloads/v4l-utils/v4l-utils-1.0.0.tar.bz2
#echo "Installing v4l"
#tar -xvf v4l-utils-1.0.0.tar.bz2
#cd v4l-utils-1.0.0/
#make
git clone git://linuxtv.org/media_build.git
cd media_build
./build
sudo make install
cd ..
echo "Downloading OpenCV 2.4.6.1"
#wget -O OpenCV-2.4.3.tar.bz2 http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.3/OpenCV-2.4.3.tar.bz2/download
echo "Installing OpenCV 2.4.6.1"
tar -xvf opencv-2.4.6.1.tar.gz
cd opencv-2.4.6.1
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D WITH_V4L=ON -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=ON -D WITH_QT=ON -D WITH_OPENGL=ON -D WITH_IPP=ON -D WITH_OPENCL=ON -D WITH_GCD=ON ..
make
sudo make install
sudo sh -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig
echo "OpenCV 2.4.6.1 ready to be used"
```


My Project Report
================

My project report on "Creating a Gesture based Mouse-Application-Controller system" can be downloaded from my [dropbox] (https://www.dropbox.com/s/x49z93j1yqymkrk/G-Mac.pdf?dl=0)


Windows Version
===============

My fellow project team member developed the windows version of G_MAC. It compiles and handle the OS collaboration with windows.h file **which I added on the master. There are saveral more features, such as windows mouse manipulation. Off course the mouse property of windows is not similer to the one in Linux; there are more like gesture based image manipulation etc. You can grab a copy from [here ..] (https://www.dropbox.com/s/htmj7c6fjy8behb/FINAL_VERSION_GMAC.rar?dl=0) 

His [Facebook contact ...] (https://www.facebook.com/niaz.munna.bd?ref=ts&fref=ts)
