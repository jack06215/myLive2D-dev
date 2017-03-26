/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#include "LAppLive2DManager.h"

#include "L2DViewMatrix.h"

//Live2DApplication
#include "LAppModel.h"
#include "LAppDefine.h"
#include "LAppModel.h"
#include "L2DMotionManager.h"

#include "PlatformManager.h"

#include "Live2DFramework.h"

using namespace live2d;
using namespace live2d::framework;

PlatformManager *s_manager;

LAppLive2DManager::LAppLive2DManager()
	:modelIndex(0)
{
	
	
	
	


	
	live2d::Live2D::init( &myAllocator );
	s_manager =new PlatformManager();
	Live2DFramework::setPlatformManager(s_manager);
	LAppModel::init();
}


LAppLive2DManager::~LAppLive2DManager() 
{
	delete s_manager;
	releaseModel();
	Live2D::dispose();
}


void LAppLive2DManager::releaseModel()
{
	for (unsigned int i=0; i<models.size(); i++)
	{
		delete models[i];
	}
    models.clear();
}


void LAppLive2DManager::setDrag(float x, float y)
{
	for (unsigned int i=0; i<models.size(); i++)
	{
		models[i]->setDrag(x, y);
	}
}



bool LAppLive2DManager::tapEvent(float x,float y)
{
	if(LAppDefine::DEBUG_LOG) UtDebug::print( "tapEvent\n");
	
	for (unsigned int i=0; i<models.size(); i++)
	{
		if(models[i]->hitTest(  HIT_AREA_HEAD,x, y ))
		{
			
			if(LAppDefine::DEBUG_LOG)UtDebug::print( "face\n");
			models[i]->setRandomExpression();
		}
		else if(models[i]->hitTest( HIT_AREA_BODY,x, y))
		{
			if(LAppDefine::DEBUG_LOG)UtDebug::print( "body\n");
			models[i]->startRandomMotion(MOTION_GROUP_TAP_BODY, PRIORITY_NORMAL );
		}
	}
	    
    return true;
}



void LAppLive2DManager::changeModel()
{
	if(LAppDefine::DEBUG_LOG)UtDebug::print("model index : %d\n",modelIndex );	
	switch (modelIndex)
	{
	case 0:
		releaseModel();
		models.push_back(new LAppModel());
		models[0]->load( MODEL_HARU ) ;
		break;
	case 1:
		releaseModel();
		models.push_back(new LAppModel());
		models[0]->load( MODEL_SHIZUKU ) ;
		break;
	case 2:
		releaseModel();
		models.push_back(new LAppModel());
		models[0]->load( MODEL_WANKO ) ;
		break;
	case 3:
		releaseModel();
		models.push_back(new LAppModel());
		models[0]->load( MODEL_HARU_A ) ;
				
		models.push_back(new LAppModel());
		models[1]->load( MODEL_HARU_B ) ;
		break;
	case 4:
		releaseModel();
		models.push_back(new LAppModel());
		models[0]->load( MODEL_EPSILON ) ;
		break;
	default:		
		break;
	}
	modelIndex++;
	modelIndex = modelIndex%5;
}

void LAppLive2DManager::deviceLost(){
	if(LAppDefine::DEBUG_LOG) live2d::UtDebug::print( "DeviceLost @LAppLive2DManager::deviceLost()\n");

	LAppModel::deviceLostCommon();
	for (unsigned int i=0; i<models.size(); i++)
	{
		models[i]->deviceLost() ;
	}
}

void LAppLive2DManager::deviceReset()
{
	LAppModel::deviceResetCommon();
}
