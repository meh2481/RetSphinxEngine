/*
    Pony48 header - webcam.h
    Copyright (c) 2014 Mark Hutcheson
*/

#ifndef WEBCAM_H
#define WEBCAM_H

#ifdef USE_VIDEOINPUT
	#include "videoInput.h"
#elif !defined(NO_WEBCAM)
	#include "opencv2/opencv.hpp"
#endif
#include "globaldefs.h"

class Webcam
{
protected:
#ifdef USE_VIDEOINPUT
	videoInput VI;
	unsigned char* m_curFrame;
	int m_device;
#elif !defined(NO_WEBCAM)
	cv::VideoCapture*	m_VideoCap;
	cv::Mat* 			m_curFrame;
#else
  char* m_VideoCap;
  char* m_curFrame; //Stub these out
#endif
	GLuint   			m_hTex;
	
	int m_iWidth, m_iHeight;
	
	void _clear();	//Clear memory associated with webcam objects
	
public:
	bool use;	//Set to false to ignore all webcam stuff (cause some people may want it off entirely)
	
	Webcam();
	~Webcam();
	
	void draw(float32 height, Point ptCenter);			//Draw the previous frame to the screen, with the specified height (width is determined by frame width)
	void getNewFrame();		//Get a new frame from the webcam
	int getFrameWidth() 	{return m_iWidth;};		//Get width of a webcam frame
	int getFrameHeight()	{return m_iHeight;};	//Get height of a webcam frame
	
	void open(int device);			//Open specified webcam
	bool isOpen();
	bool saveFrame(string sFilename, bool bMirror = true);	//Save the current webcam frame to a file
	
	bool mirror;					//If we should mirror-image this when drawing or not
};






































#endif	//defined WEBCAM_H
 
