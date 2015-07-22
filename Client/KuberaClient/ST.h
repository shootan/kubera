
#pragma once
#include "header.h"

class ST
{
private:
	ST()
	{
		m_bVisible =FALSE;
		m_bSelected = FALSE;
		m_bStart = FALSE;
	}
public:
	~ST()
	{
		delete instance;
	}

	BOOL m_bVisible;
	BOOL m_bSelected;
	BOOL m_bStart;
public:
	static ST* instance;
	static ST* sharedManager()
	{
		if(instance == NULL) instance = new ST;
		return instance;
	}

private:

public:
	float GetDistance(Vector3 f_pos, Vector3 s_pos)
	{
		Vector3 v;
		v.x = f_pos.x - s_pos.x;
		v.y = f_pos.y - s_pos.y;
		v.z = f_pos.z - s_pos.z;
		float distance = sqrtf(v.x*v.x + v.z*v.z);
		return distance;
	}

	Vector3 GetVector(Vector3 f_pos, Vector3 s_pos)
	{
		Vector3 v;
		v.x = f_pos.x - s_pos.x;
		v.y = f_pos.y - s_pos.y;
		v.z = f_pos.z - s_pos.z;

		float distance = GetDistance(f_pos, s_pos);

		v.x /= distance;
		v.z /= distance;

		return v;
	}
};