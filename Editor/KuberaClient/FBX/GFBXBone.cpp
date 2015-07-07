#include "DXUT.h"
#include "GFBXBone.h"

namespace GFBX {


BoneKeyFrame::BoneKeyFrame()
{
	D3DXMatrixIdentity(&mGlobalBindposeInverse);
	D3DXMatrixIdentity(&mGlobalBindpose);
}

BoneKeyFrame::~BoneKeyFrame()
{
}





Bone::Bone()
{
}


Bone::~Bone()
{

}



};