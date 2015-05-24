#pragma once

#include <vector>

namespace GFBX {

class BoneKeyFrame
{
public:
	BoneKeyFrame();
	virtual ~BoneKeyFrame();

	D3DXMATRIX mGlobalBindposeInverse;
	D3DXMATRIX mGlobalBindpose;
};


class Bone
{
public:
	Bone();
	virtual ~Bone();

	std::string boneName;

	std::vector<BoneKeyFrame> frames;

};

};
