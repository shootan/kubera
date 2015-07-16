#pragma once
#include "MathHelper.h"
#include <vector>
#include <algorithm>


namespace GFBX {

struct PNTVertex
{
	D3DXVECTOR3 mPosition;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR2 mUV;

	bool operator==(const PNTVertex& rhs) const
	{
		
		if (this->mPosition != rhs.mPosition)
			return false;
		if (this->mNormal != rhs.mNormal)
			return false;
		if (this->mUV != rhs.mUV)
			return false;

		return true;
	}
};

struct VertexBlendingInfo
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	VertexBlendingInfo() :
		mBlendingIndex(0),
		mBlendingWeight(0.0)
	{}

	bool operator < (const VertexBlendingInfo& rhs)
	{
		return (mBlendingWeight > rhs.mBlendingWeight);
	}
};

struct PNTIWVertex
{
	D3DXVECTOR3 mPosition;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR2 mUV;
	D3DXVECTOR3 mTangent;
	D3DXVECTOR3 mBinormal;
	std::vector<VertexBlendingInfo> mVertexBlendingInfos;

	void SortBlendingInfoByWeight()
	{
		std::sort(mVertexBlendingInfos.begin(), mVertexBlendingInfos.end());
	}

	bool operator==(const PNTIWVertex& rhs) const
	{
		bool sameBlendingInfo = true;

		// We only compare the blending info when there is blending info
		if (!(mVertexBlendingInfos.empty() && rhs.mVertexBlendingInfos.empty()))
		{
			// Each vertex should only have 4 index-weight blending info pairs
			for (unsigned int i = 0; i < 4; ++i)
			{
				if (mVertexBlendingInfos[i].mBlendingIndex != rhs.mVertexBlendingInfos[i].mBlendingIndex ||
					abs(mVertexBlendingInfos[i].mBlendingWeight - rhs.mVertexBlendingInfos[i].mBlendingWeight) > 0.001)
				{
					sameBlendingInfo = false;
					break;
				}
			}
		}

		bool result1 = MathHelper::CompareVector3WithEpsilon(mPosition, rhs.mPosition);
		bool result2 = MathHelper::CompareVector3WithEpsilon(mNormal, rhs.mNormal);
		bool result3 = MathHelper::CompareVector2WithEpsilon(mUV, rhs.mUV);
		bool result4 = MathHelper::CompareVector3WithEpsilon(mTangent, rhs.mTangent);
		bool result5 = MathHelper::CompareVector3WithEpsilon(mBinormal, rhs.mBinormal);

		return result1 && result2 && result3 && result4 && result5 && sameBlendingInfo;
	}
};

};