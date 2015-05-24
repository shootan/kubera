#include "DXUT.h"
#include "MathHelper.h"

namespace GFBX {

const D3DXVECTOR2 MathHelper::vector2Epsilon = D3DXVECTOR2(0.00001f, 0.00001f);
const D3DXVECTOR3 MathHelper::vector3Epsilon = D3DXVECTOR3(0.00001f, 0.00001f, 0.00001f);
#define VECTOR_EPSILON	(0.00001f)


bool MathHelper::CompareScalarWithEpsilon(const float lhs, const float rhs, const float eps)
{
	if (abs(lhs - rhs) < eps)
		return true;
	else
		return false;
}

bool MathHelper::CompareVector3WithEpsilon(const D3DXVECTOR3& lhs, const D3DXVECTOR3& rhs)
{
	return CompareScalarWithEpsilon(lhs.x, rhs.x, VECTOR_EPSILON)
		&& CompareScalarWithEpsilon(lhs.y, rhs.y, VECTOR_EPSILON)
		&& CompareScalarWithEpsilon(lhs.z, rhs.z, VECTOR_EPSILON);
}

bool MathHelper::CompareVector2WithEpsilon(const D3DXVECTOR2& lhs, const D3DXVECTOR2& rhs)
{
	return CompareScalarWithEpsilon(lhs.x, rhs.x, VECTOR_EPSILON)
		&& CompareScalarWithEpsilon(lhs.y, rhs.y, VECTOR_EPSILON);

}

}