#include "robustSinglePointerTracking.cpp"

#include <iostream>
#include <algorithm>
#include <stdio.h>	
#include <cv.h>
#define _USR_MATH_DEFINES
#include <cmath>
#include <highgui.h>
#include <bits/stdc++.h>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cassert>

using namespace std;

//constants for sliders
int sens = 5, ncomp = 27, brightnessThresh = 200;
int hMin= 0, hMax = 9, sMin = 0, sMax = 100, vMin = 240, vMax = 255;
int theta = 8900, f = 800.0*352/640;//focal length
int yThresh = 10;

//for serial communication
struct termios tio;
int tty_fd = -1;
//initialize the serial ports 
int initSerial(){
	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag=0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;
	int i=0;
	while(tty_fd == -1 && i<10){
		char usb0[50];
		sprintf(usb0,"/dev/ttyUSB%d",i);
		tty_fd=open(usb0, O_RDWR | O_NOCTTY | O_NDELAY);      
		i++;
	}

	assert(tty_fd!=-1);
	tcflush(tty_fd,TCIOFLUSH);
	cfsetospeed(&tio,B115200);            // 115200 baud
	cfsetispeed(&tio,B115200);            // 115200 baud
}
//send a particular instruction
void sendInstruction(char curr_instruction, int instr_val){
		char ins;
		if(curr_instruction == 'R'){
			ins=abs(instr_val);
			ins |= (1<<7);
			if(instr_val<0) ins |= (1<<6);
		}
		else{
			ins = (char) instr_val;
		}
		printf("Sending\n");
		write(tty_fd,&ins,1);                     // if new data is available on the console, send it to the serial port
		printf("%c\n",(int)ins);
		char c = 0;
		//wait till it is ACKed using an S
		while(c!='S'){
			read(tty_fd, &c,1);
			cout<<"reply1: "<<(int)c<<endl; 
		}
		//ins=0;
		char tmp=0;
		//send dummy data which signals end of communication
		write(tty_fd,&tmp,1);                     // if new data is available on the console, send it to the serial port
		c = 0;
		//wait till it is ACKed using an U
		while(c!='U'){
			read(tty_fd, &c,1);
			cout<<"reply2: "<<(int)c<<endl; 
		}
}

	
int main(int argc, char *argv[]){

	//352x288
	initSerial();
	//reduce resolution to fit USB bandwidth
	float width = 352.0, height = 288.0;
    VideoCapture cap1(0);        // give camera number
	if(cap1.set(CV_CAP_PROP_FRAME_WIDTH, width)==0){
		cerr<<"error in resizing";
	}
	if(cap1.set(CV_CAP_PROP_FRAME_HEIGHT, height)==0){
		cerr<<"error in resizing";
	}
    VideoCapture cap2(1); 
	if(cap2.set(CV_CAP_PROP_FRAME_WIDTH, width)==0){
		cerr<<"error in resizing";
	}
	if(cap2.set(CV_CAP_PROP_FRAME_HEIGHT, height)==0){
		cerr<<"error in resizing";
	}
    
    if(!cap1.isOpened()){        // check if we succeeded
        printf("could not open capture device 1\n");
        return -1;
    }
    if(!cap2.isOpened()){        // check if we succeeded
        printf("could not open capture device 2\n");
        return -1;
    }


    //setup display indow
    namedWindow("PointerTracking1",CV_WINDOW_AUTOSIZE);
    namedWindow("PointerTracking2",CV_WINDOW_AUTOSIZE);
    namedWindow("mask",CV_WINDOW_AUTOSIZE);

	//trackbar creating code
    createTrackbar("hMin", "PointerTracking1", &hMin, 179);
    createTrackbar("hMax", "PointerTracking1", &hMax, 179);
    createTrackbar("sMin", "PointerTracking1", &sMin, 255);
    createTrackbar("sMax", "PointerTracking1", &sMax, 255);
    createTrackbar("vMin", "PointerTracking1", &vMin, 255);
    createTrackbar("vMax", "PointerTracking1", &vMax, 255);
    createTrackbar("f", "PointerTracking1", &f, 1200);
    createTrackbar("theta*10(degrees)", "PointerTracking1", &theta, 18000);

    //fill buffer with N frames, not of any real significance as this is just legacy code
    printf("filling buffer...\n");
    int N = 2;
    std::vector<Mat> history;//, history2;
    Mat frame, frame1;//, frame1p, frame2p;
    while(history.size() < N){
        cap1 >> frame;
        history.push_back(frame.clone());  
        
        cap2 >> frame;
        history.push_back(frame.clone());       
    }

    Mat rgb_channels[3];
    Mat rgb_channels2[3];
    Mat hsv=frame.clone();
    Mat mask = frame.clone();
    Moments m;
    int window = 11;
    int windowCount = 0;
	//buffered z and x coordinates
	float zbuffer[11] = {0,0,0,0,0,0,0,0,0,0,0};
	float xbuffer[11] = {0,0,0,0,0,0,0,0,0,0,0};

    while(true){
	    cout<<"Ha"<<endl;
        cap1 >> frame;
		//HSV filtering
		cvtColor(frame, hsv, CV_BGR2HSV);
		inRange(hsv, Scalar(hMin,sMin,vMin), Scalar(hMax,sMax,vMax), mask);
		//calculate centroid
		m = moments(mask, true);	
		Point pt1(m.m10/m.m00, m.m01/m.m00);

        if(!(pt1.x == -1 || pt1.y == -1)){
            circle(frame,pt1,5,CV_RGB(255,0,0),3); 
        }
        else{
            printf("Degenerate point\n"); 
        }

		//camera 1 with circle overlayed
        imshow("PointerTracking1",frame);           
		//masked 
        imshow("mask",mask);           

		cap2 >> frame;
		//HSV filtering
		cvtColor(frame, hsv, CV_BGR2HSV);
		inRange(hsv, Scalar(hMin,sMin,vMin), Scalar(hMax,sMax,vMax), mask);
		//calculate centroid
		m = moments(mask,true);	
		Point pt2(m.m10/m.m00, m.m01/m.m00);

		if(!(pt2.x == -1 || pt2.y == -1)){
			circle(frame,pt2,5,CV_RGB(255,0,0),3); 
		}
		else{
			printf("Degenerate point\n");  
		}
		imshow("PointerTracking2",frame);      

		//distance between cameras
		float t = 4;
		//if the y coordinates are close enough (epipolar constraint) and the points are detected then push z and x coordinates into buffer
		if(pt1.x >=0 && pt1.y >=0 && pt2.y>=0 && pt2.x>=0 && (pt2.y - pt1.y)<=yThresh){
			float x2 = pt2.x - width/2;
			float x1 = pt1.x - width/2;
			float z = t/abs(tan(M_PI*(theta/100.0-90)/180.0 - atan(x2/f)) + x1/f);
			zbuffer[windowCount]=z;
			xbuffer[windowCount++]=(x1+(float)x2)/2;
		}
		//else reset buffer
		else{
			windowCount = 0;
		}


		//buffer is full, start processing through median
		if(windowCount ==window){
			//sorting
			sort(zbuffer,zbuffer+10);
			sort(xbuffer,xbuffer+10);
			//median
			int z = zbuffer[5];
			int x = xbuffer[5]/(float)f * z;
			x+=5;
			cout<<"PointDetected z:"<<z <<" x:"<< x <<endl;
			//get users choice
			char choice;
			cin>>choice;
			//send command if choice is yes
			if(choice=='y'){
				while(z>127){
					sendInstruction('F', 127);
					z-=127;
				}
				sendInstruction('F', z);
				if(abs(x)> 6){
					if(x<0){
						sendInstruction('R', 45);
					}
					if(x>0){
						sendInstruction('R', -45);
					}
					sendInstruction('F', abs(x));
				}
			}
			//reset buffer
			windowCount = 0;
		}
		if(waitKey(10) % 0x100 == 27){
			break;
		}
    }
    return(0);
}
