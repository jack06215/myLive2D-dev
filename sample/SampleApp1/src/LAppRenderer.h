/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#pragma once

#include "L2DViewMatrix.h"
#include "L2DTargetPoint.h"

class LAppLive2DManager;



class LAppRenderer
{
private:
	LAppLive2DManager*			live2DMgr;

	live2d::framework::L2DViewMatrix				viewMatrix;
	live2d::framework::L2DMatrix44 				deviceToScreen;
public:
	LAppRenderer();
	~LAppRenderer();
	void setLive2DManager(LAppLive2DManager* mgr);
	void draw();

	void scaleView(float cx,float cy,float scale);
    void translateView(float x,float y); 

	void mousePress( int x , int y ) ;
	void mouseDrag(int x,int y);

	void mouseWheel( int delta , int x , int y ) ;
	void updateViewMatrix( float dx , float dy , float cx , float cy , float scale ) ;

	float transformDeviceToViewX(float deviceX);
	float transformDeviceToViewY(float deviceY);

	void setDeviceSize( int width , int height ) ;

	live2d::framework::L2DViewMatrix* getViewMatrix(){ return &viewMatrix ; } 
};




