#include <Windows.h>
#include <stdint.h>
#include <xnamath.h>

namespace GFBX {

class MathHelper
{
public:

	static const D3DXVECTOR3 vector3Epsilon;
	static const D3DXVECTOR2 vector2Epsilon;
	static const D3DXVECTOR3 vector3True;
	static const D3DXVECTOR2 vector2True;

	static bool CompareVector2WithEpsilon(const D3DXVECTOR2& lhs, const D3DXVECTOR2& rhs);
	static bool CompareVector3WithEpsilon(const D3DXVECTOR3& lhs, const D3DXVECTOR3& rhs);
	static bool CompareScalarWithEpsilon(const float lhs, const float rhs, const float eps);
};

};