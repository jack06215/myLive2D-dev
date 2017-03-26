/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#pragma once

#include "L2DBaseModel.h"
#include "Live2DModelD3D.h"
#include <vector>
#include <string>

#include "ModelSetting.h"
#include "L2DViewMatrix.h"

class LAppModel : public live2d::framework::L2DBaseModel
{
private:
    ModelSetting*				modelSetting;
	std::string			modelHomeDir;

public:
	static void init();
	static void deviceLostCommon();
	static void deviceResetCommon();

public:
    LAppModel();
    ~LAppModel(void);
    
    void load(int modelIndex);
	void load(const char* path) ;

    void update();
    void draw();
	
    int startMotion(const char name[],int no,int priority);
	int startRandomMotion(const char name[],int priority);
	
	void setExpression(const char name[]);
	void setRandomExpression();
	
	void preloadMotionGroup(const char name[]);
    
	bool hitTest(const char pid[],float testX,float testY);

	void deviceLost() ;
};






