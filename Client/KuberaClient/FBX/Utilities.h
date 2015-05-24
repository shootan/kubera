#pragma once

#include <fbxsdk.h>
#include <iostream>
#include <string>
#include <vector>
//#include "Vertex.h"

namespace GFBX {


	struct FbxBlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	FbxBlendingIndexWeightPair() :
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

// Each Control Point in FBX is basically a vertex
// in the physical world. For example, a cube has 8
// vertices(Control Points) in FBX
// Joints are associated with Control Points in FBX
// The mapping is one joint corresponding to 4
// Control Points(Reverse of what is done in a game engine)
// As a result, this struct stores a D3DXVECTOR3 and a 
// vector of joint indices
struct FbxCtrlPoint
{
	D3DXVECTOR3 mPosition;
	std::vector<FbxBlendingIndexWeightPair> mBlendingInfo;

	FbxCtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};

// This stores the information of each key frame of each joint
// This is a linked list and each node is a snapshot of the
// global transformation of the joint at a certain frame
struct FbxKeyframe
{
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	FbxKeyframe* mNext;

	FbxKeyframe() :
		mNext(nullptr)
	{}
};

// This is the actual representation of a joint in a game engine
struct FbxJoint
{
	std::string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	FbxAMatrix mGlobalBindpose;
	FbxKeyframe* mAnimation;
	FbxNode* mNode;

	FbxJoint() :
		mNode(nullptr),
		mAnimation(nullptr)
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
	}

	~FbxJoint()
	{
		while (mAnimation)
		{
			FbxKeyframe* temp = mAnimation->mNext;
			delete mAnimation;
			mAnimation = temp;
		}
	}
};

struct FbxSkeleton
{
	std::vector<FbxJoint> mJoints;
};

struct FbxTriangle
{
	std::vector<unsigned int> mIndices;
	std::string mMaterialName;
	unsigned int mMaterialIndex;

	bool operator<(const FbxTriangle& rhs)
	{
		return mMaterialIndex < rhs.mMaterialIndex;
	}

	void clear()
	{
		mIndices.clear();
		mMaterialName.clear();
		mMaterialIndex = 0;
	}
};


class Utilities
{
public:

	// This function should be changed if exporting to another format
	static void WriteMatrix(std::ostream& inStream, FbxMatrix& inMatrix, bool inIsRoot);

	static void PrintMatrix(FbxMatrix& inMatrix);

	static FbxAMatrix GetGeometryTransformation(FbxNode* inNode);

	static std::string GetFileName(const std::string& inInput);

	static std::string RemoveSuffix(const std::string& inInput);

	static void ConvertToD3DXMatrix(FbxMatrix& inMatrix, D3DXMATRIX& outMatrix);
};





};