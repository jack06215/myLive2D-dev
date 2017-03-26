/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */
#include "LAppTextureDesc.h"


LAppTextureDesc::LAppTextureDesc(LPDIRECT3DTEXTURE9 tex)
{
	data=tex;
}

LAppTextureDesc::~LAppTextureDesc()
{
	data->Release();
}
