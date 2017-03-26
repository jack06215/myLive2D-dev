/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#pragma once

#include "type/LDVector.h"
#include <math.h>
#include "MyLive2DAllocator.h"


class LAppModel;
class L2DViewMatrix;

class LAppLive2DManager{
private :
	
	live2d::LDVector<LAppModel*> models;
	
	
	int modelIndex;

	MyLive2DAllocator	myAllocator ;
public:
    
    LAppLive2DManager() ;    
    ~LAppLive2DManager() ; 
    
	void init();
	void releaseModel();
    LAppModel* getModel(int no){ return models[no]; }
    int getModelNum(){return models.size();}
    bool tapEvent(float x,float y) ;
    void setDrag(float x, float y);
	void changeModel();

	void deviceLost() ;
	void deviceReset();
};

