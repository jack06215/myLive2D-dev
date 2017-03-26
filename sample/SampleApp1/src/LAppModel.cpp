/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */

#include <string>


//Live2D Application
#include "LAppModel.h"
#include "LAppDefine.h"

#include "FileManager.h"
#include "ModelSettingJson.h"
#include "util\UtSystem.h"

#include "L2DStandardID.h"

#include <sstream> 

using namespace std;
using namespace live2d;
using namespace live2d::framework;



extern LPDIRECT3DDEVICE9		g_pD3DDevice ;


extern LPDIRECT3DTEXTURE9		g_pMaskTexture ;


extern LPDIRECT3DSURFACE9		g_pMaskBufferSurface ;

void LAppModel::init()
{
	
	
	Live2DModelD3D::setDevice(g_pD3DDevice);
	Live2DModelD3D::setMaskSurface(g_pMaskBufferSurface);
	Live2DModelD3D::setMaskTexture(g_pMaskTexture);
}

void LAppModel::deviceLostCommon()
{
	
	Live2DModelD3D::deviceLostCommon();
}

void LAppModel::deviceResetCommon()
{
	
	Live2DModelD3D::deviceResetCommon();

	
	Live2DModelD3D::setMaskSurface(g_pMaskBufferSurface);
	Live2DModelD3D::setMaskTexture(g_pMaskTexture);
}

LAppModel::LAppModel()
	:L2DBaseModel(),modelSetting(NULL)
{
	if (LAppDefine::DEBUG_LOG)
	{
		mainMotionMgr->setMotionDebugMode(true);
	}
}


LAppModel::~LAppModel(void)
{
	if(LAppDefine::DEBUG_LOG)UtDebug::print("delete model\n");
	delete modelSetting;
}


void LAppModel::load(int modelIndex)
{
	const char* path = NULL ;
	switch (modelIndex)
	{
	case MODEL_HARU:
		path = "res\\live2d\\haru\\haru.model.json" ;
		break;
	case MODEL_SHIZUKU:
		path = "res\\live2d\\shizuku\\shizuku.model.json" ;
		break;
	case MODEL_WANKO:
		path = "res\\live2d\\wanko\\wanko.model.json" ;
		break;
	case MODEL_HARU_A:
		path = "res\\live2d\\haru\\haru_01.model.json" ;
		break;
	case MODEL_HARU_B:
		path = "res\\live2d\\haru\\haru_02.model.json" ;
		break;
	case MODEL_EPSILON:
		// Cubism SDK2.1ƒ‚ƒfƒ‹
		path = "res\\live2d\\epsilon\\Epsilon2.1.model.json" ;
		break;
	default:		
		break;
	}

	if( path == NULL ){
		UtDebug::print( "Not supported model no : %d @ LAppModel::load()\n",modelIndex );	
		return ;
	}

	load(path) ;
}


void LAppModel::load(const char* path)
{

	if(LAppDefine::DEBUG_LOG) UtDebug::print( "load model : %s\n",path);	
    updating=true;
    initialized=false;
    
	int size ;

	unsigned char* data = FileManager::loadFile( path , &size ) ;
    modelSetting = new ModelSettingJson( (char*)data , size );

	FileManager::releaseBuffer(data);
	
	
	FileManager::getParentDir( path , &modelHomeDir ) ;

    if(LAppDefine::DEBUG_LOG) UtDebug::print( "create model : %s\n",modelSetting->getModelName());	
    updating=true;
    initialized=false;

   
    if( strcmp( modelSetting->getModelFile() , "" ) != 0 )
    {        
        string path=modelSetting->getModelFile();
		path=modelHomeDir+ path;
        loadModelData(path.c_str());
        
		int len=modelSetting->getTextureNum();
		for (int i=0; i<len; i++)
		{
			string texturePath=modelSetting->getTextureFile(i);
			texturePath=modelHomeDir+texturePath;
			loadTexture(i,texturePath.c_str());
		}
    }
	
	if (live2DModel==NULL) {

		return;
	}

     //Expression
	if (modelSetting->getExpressionNum() > 0)
	{
		int len=modelSetting->getExpressionNum();
		for (int i=0; i<len; i++)
		{
			string name=modelSetting->getExpressionName(i);
			string file=modelSetting->getExpressionFile(i);
			file=modelHomeDir+file;
			loadExpression(name.c_str(),file.c_str());
		}
	}
	
	//Physics
	if( strcmp( modelSetting->getPhysicsFile(), "" ) != 0 )
    {
		string path=modelSetting->getPhysicsFile();
		path=modelHomeDir+path;
        loadPhysics(path.c_str());
    }
	
	//Pose
	if( strcmp( modelSetting->getPoseFile() , "" ) != 0 )
    {
		string path=modelSetting->getPoseFile();
		path=modelHomeDir+path;
        loadPose(path.c_str());
    }

	
	if (eyeBlink==NULL)
	{
		eyeBlink=new L2DEyeBlink();
	}
	
	//Layout
	map<string, float> layout;
	modelSetting->getLayout(layout);
	modelMatrix->setupLayout(layout);
	
	for ( int i = 0; i < modelSetting->getInitParamNum(); i++)
	{
		live2DModel->setParamFloat(modelSetting->getInitParamID(i), modelSetting->getInitParamValue(i));
	}

	for ( int i = 0; i < modelSetting->getInitPartsVisibleNum(); i++)
	{
		live2DModel->setPartsOpacity(modelSetting->getInitPartsVisibleID(i), modelSetting->getInitPartsVisibleValue(i));
	}
	
	live2DModel->saveParam();

	preloadMotionGroup(MOTION_GROUP_IDLE);
	
	mainMotionMgr->stopAllMotions();
	
    updating=false;
    initialized=true;
}


void LAppModel::preloadMotionGroup(const char group[])
{
    int len = modelSetting->getMotionNum( group );
    for (int i = 0; i < len; i++)
	{
		std::stringstream ss;
		
		//ex) idle_0
		ss << group << "_" <<  i;
		
		string name=ss.str();
		string path=modelSetting->getMotionFile(group,i);
		path=modelHomeDir+path;

		if(LAppDefine::DEBUG_LOG)UtDebug::print("load motion name:%s ",name.c_str());
        
		AMotion* motion=loadMotion(name.c_str(),path.c_str());
    }
}


void LAppModel::update()
{
	dragMgr->update();
	dragX=dragMgr->getX();
	dragY=dragMgr->getY();

	//-----------------------------------------------------------------
	live2DModel->loadParam();
	if(mainMotionMgr->isFinished())
	{
		
		startRandomMotion(MOTION_GROUP_IDLE, PRIORITY_IDLE);
	}
	else
	{
		bool update = mainMotionMgr->updateParam(live2DModel);
		
		if( ! update){
			
			eyeBlink->setParam(live2DModel);
		}
	}
	live2DModel->saveParam();
	//-----------------------------------------------------------------
	
	
	if(expressionMgr!=NULL)expressionMgr->updateParam(live2DModel);
	
	
	
	live2DModel->addToParamFloat( PARAM_ANGLE_X, dragX *  30 , 1 );
	live2DModel->addToParamFloat( PARAM_ANGLE_Y, dragY *  30 , 1 );
	live2DModel->addToParamFloat( PARAM_ANGLE_Z, (dragX*dragY) * -30 , 1 );
	
	
	live2DModel->addToParamFloat( PARAM_BODY_ANGLE_X    , dragX * 10 , 1 );
	
	
	live2DModel->addToParamFloat( PARAM_EYE_BALL_X, dragX  , 1 );
	live2DModel->addToParamFloat( PARAM_EYE_BALL_Y, dragY  , 1 );
	
	
	LDint64	 timeMSec = UtSystem::getUserTimeMSec() - startTimeMSec  ;
	double t = (timeMSec / 1000.0) * 2 * 3.14159  ;//2*Pi*t
	
	live2DModel->addToParamFloat( PARAM_ANGLE_X,	(float) (15 * sin( t/ 6.5345 )) , 0.5f);
	live2DModel->addToParamFloat( PARAM_ANGLE_Y,	(float) ( 8 * sin( t/ 3.5345 )) , 0.5f);
	live2DModel->addToParamFloat( PARAM_ANGLE_Z,	(float) (10 * sin( t/ 5.5345 )) , 0.5f);
	live2DModel->addToParamFloat( PARAM_BODY_ANGLE_X,	(float) ( 4 * sin( t/15.5345 )) , 0.5f);
	live2DModel->setParamFloat  ( PARAM_BREATH,	(float) (0.5f + 0.5f * sin( t/3.2345 )),1);
	
	
	if(physics!=NULL)physics->updateParam(live2DModel);

	
	if(lipSync)
	{
		float value = 0;
		live2DModel->setParamFloat(PARAM_MOUTH_OPEN_Y, value ,0.8f);
	}
	
	
	if(pose!=NULL)pose->updateParam(live2DModel);

	live2DModel->update();
}


int LAppModel::startMotion(const char group[],int no,int priority)
{
	if (priority==PRIORITY_FORCE)
	{
		mainMotionMgr->setReservePriority(priority);
	}
	else if (! mainMotionMgr->reserveMotion(priority))
	{
		if(LAppDefine::DEBUG_LOG)UtDebug::print("can't start motion.\n");
		return -1;
	}
	
	const char* fileName = modelSetting->getMotionFile(group, no);
	std::stringstream ss;
	
	//ex) idle_0
	ss << group << "_" <<  no;
	
	string name=ss.str();
	AMotion* motion = motions[name.c_str()];
	bool autoDelete = false;
	if ( motion == NULL )
	{
		
		string path=fileName;
		path=modelHomeDir+path;
		motion = loadMotion(NULL,path.c_str());
		
		autoDelete = true;
	}
	
	motion->setFadeIn(  modelSetting->getMotionFadeIn(group,no)  );
	motion->setFadeOut( modelSetting->getMotionFadeOut(group,no) );
	
    if(LAppDefine::DEBUG_LOG)UtDebug::print("start motion ( %s : %d )",group,no);
	return mainMotionMgr->startMotionPrio(motion,autoDelete,priority);
}


int LAppModel::startRandomMotion(const char name[],int priority)
{
	if(modelSetting->getMotionNum(name)==0)return -1;
    int no = rand() % modelSetting->getMotionNum(name); 
    
    return startMotion(name,no,priority);
}



void LAppModel::draw()
{
    if (live2DModel == NULL)return;

	
	D3DXMATRIXA16 buf ;
	g_pD3DDevice->GetTransform(D3DTS_WORLD, &buf);

	
	float* tr = modelMatrix->getArray() ;//float[16]
	g_pD3DDevice->MultiplyTransform( D3DTS_WORLD , (D3DXMATRIXA16*)tr ) ;

	
	live2DModel->draw();

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &buf);
}



bool LAppModel::hitTest(const char pid[],float testX,float testY)
{
	if(alpha<1)return false;
	int len=modelSetting->getHitAreasNum();
	for (int i = 0; i < len; i++)
	{
		if( strcmp( modelSetting->getHitAreaName(i) ,pid) == 0 )
		{
			const char* drawID=modelSetting->getHitAreaID(i);
			return hitTestSimple(drawID,testX,testY);
		}
	}
	return false;
}


void LAppModel::setExpression(const char expressionID[])
{
	AMotion* motion = expressions[expressionID] ;
	if(LAppDefine::DEBUG_LOG)UtDebug::print( "expression[%s]\n" , expressionID ) ;
	if( motion != NULL )
	{
		expressionMgr->startMotion(motion, false) ;
	}
	else
	{
		if(LAppDefine::DEBUG_LOG)UtDebug::print( "expression[%s] is null \n" , expressionID ) ;
	}
}


void LAppModel::setRandomExpression()
{
	int no=rand()%expressions.size();
	map<string,AMotion* >::const_iterator map_ite;
	int i=0;
	for(map_ite=expressions.begin();map_ite!=expressions.end();map_ite++)
	{
		if (i==no)
		{
			string name=(*map_ite).first;
			setExpression(name.c_str());
			return;
		}
		i++;
	}
}


void LAppModel::deviceLost() {
	((Live2DModelD3D*)live2DModel)->deviceLostD3D() ;
}
