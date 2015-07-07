#pragma once

#include "GFBXBone.h"
#include <vector>

namespace GFBX {


struct VertexBonePoint
{
	D3DXVECTOR3 pos;
	enum _FVF { FVF = (D3DFVF_XYZ) };
};


class Skeleton
{
public:
	Skeleton();
	virtual ~Skeleton();

	std::vector<Bone> bones;
	std::string animationName;
	int animationLength;

	HRESULT DrawBones(IDirect3DDevice9* pd3dDevice, const D3DXMATRIX* mWorld, 
		const D3DXMATRIX* mView, const D3DXMATRIX* mProj, float t, float boneRadius);
	int GetBoneCount() { return bones.size(); }
	D3DXMATRIX GetBoneTransform(int boneIdx, float t);
};

};
