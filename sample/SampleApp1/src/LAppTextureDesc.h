/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#pragma once

#include "L2DTextureDesc.h"
#include "Live2DModelD3D.h"

class LAppTextureDesc : public live2d::framework::L2DTextureDesc
{
public:
	LAppTextureDesc(LPDIRECT3DTEXTURE9 tex);
	virtual ~LAppTextureDesc();

private:
	LPDIRECT3DTEXTURE9 data;
};
