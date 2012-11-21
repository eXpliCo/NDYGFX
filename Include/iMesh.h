#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

enum COLOR
{
	NULL_COLOR = 0,
	WHITE_COLOR = 1,
	BLACK_COLOR = 2,
	RED_COLOR = 3,
	GREEN_COLOR = 4,
	BLUE_COLOR = 5,
	YELLOW_COLOR = 6,
	CYAN_COLOR = 7,
	MAGENTA_COLOR = 8
};

extern "C"
{
	class DECLDIR iMesh
	{	
	public:
		iMesh(Vector3 pos);
		virtual ~iMesh();

		virtual void SetSpecialColor(COLOR specialColor) = 0;
		/*! Sets the level of transparency. Range is [0,1]. */
		//void SetTransparency(float transparency);
		virtual void SetPosition(D3DXVECTOR3 pos) = 0;
		virtual void SetQuaternion(D3DXQUATERNION quat) = 0;
		virtual void MoveBy(D3DXVECTOR3 moveby) = 0;
		virtual void Rotate(D3DXVECTOR3 radians) = 0;
		virtual void Rotate(D3DXQUATERNION quat) = 0;
		virtual void RotateAxis(D3DXVECTOR3 around, float angle) = 0;
		virtual void Scale(D3DXVECTOR3 scale) = 0;
		virtual void Scale(float scale) = 0;

		virtual Vector3 GetPosition() const = 0;
		virtual Vector3 GetRotation() const = 0;
		virtual Vector3 GetScaling() const = 0;
	};
}
