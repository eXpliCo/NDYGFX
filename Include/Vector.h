#ifndef MALOWVECTOR
#define MALOWVECTOR

#include <math.h>

// Edit 2012-11-18 by Alexivan - Removed DX dependencies
// Edit 2012-11-23 by Alexivan - Added DX Conversions
// EDIT 2012-12-17 by Tillman - Added GetD3DXVECTORX and Union & []-operator overloading.
// EDIT 2012-12-18 by Alexivan - Warning Ignore for nameless struct in union
// EDIT 2012-12-18 by Alexivan - GetLength function made constant

#pragma warning ( push ) 
#pragma warning ( disable : 4201 ) // nonstandard extension used : nameless struct/union
#pragma warning ( disable : 4290 ) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)

class Vector2
{
public:
	union
	{
		//the variable "values" and x and y share the same memory
		float values[2];
		struct
		{
			float x; //values[0]
			float y; //values[1]
		};
	};

	Vector2()
	{
		this->x = 0;
		this->y = 0;
	}

	Vector2(float _x, float _y)
	{
		this->x = _x;
		this->y = _y;
	}

	virtual ~Vector2() {}

	float GetLength() const
	{
		return sqrt(pow(this->x, 2) + pow(this->y, 2));
	}

	void normalize()
	{
		float length = this->GetLength();
		this->x /= length;
		this->y /= length;
	}

	inline float& operator[]( unsigned int i ) throw(const char*)
	{
		if(i > 1)
		{
			throw("index out of bounds");
		}
		return values[i];
	}
	inline const float& operator[]( unsigned int i ) const
	{
		if(i > 1)
		{
			throw("index out of bounds");
		}
		return values[i];
	}

};


class Vector3
{
public:
	union
	{
		//the variable "values" and x,y and z share the same memory
		float values[3];
		struct
		{
			float x; //values[0]
			float y; //values[1]
			float z; //values[2]
		};
	};

	Vector3()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vector3(float _x, float _y, float _z)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	virtual ~Vector3() {}

	float GetLength() const
	{
		return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
	}

	void normalize()
	{
		float length = this->GetLength();
			
		if(length > 0.0f)
		{
			this->x /= length;
			this->y /= length;
			this->z /= length;
		}
	}

	float GetDotProduct(Vector3& compObj)
	{
		float dot = this->x * compObj.x;
		dot += this->y * compObj.y;
		dot += this->z * compObj.z;
		return dot;
	}

	Vector3 GetCrossProduct(Vector3 vec)
	{
		Vector3 retVec;
		retVec.x = this->y * vec.z - vec.y * this->z;
		retVec.y = -(this->x * vec.z - vec.x * this->z);
		retVec.z = this->x * vec.y - vec.x * this->y;

		return retVec;
	}

	float GetAngle(Vector3& compObj)
	{
		return acos(this->GetDotProduct(compObj) / (this->GetLength() * compObj.GetLength()));
	}
	// new for physics
		
	Vector3 operator+(const Vector3& v) const
    {
        return Vector3(this->x+v.x, this->y+v.y, this->z+v.z);
    }
	Vector3 operator-(const Vector3& v) const
	{
		return Vector3(this->x-v.x, this->y-v.y, this->z-v.z);
	}
	Vector3 operator*(const float& scalar) const
	{
		return Vector3(this->x*scalar, this->y*scalar, this->z*scalar);
	}
	Vector3 operator/(const float& scalar) const
	{
		return Vector3(this->x/scalar, this->y/scalar, this->z/scalar);
	}
	void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }
	void operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }
	void operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }
	float GetLengthSquared()
	{
		return this->GetDotProduct(*this);
	}
	Vector3 GetComponentMultiplication(const Vector3 & compVec)
	{
		return Vector3(this->x*compVec.x, this->y*compVec.y, this->z*compVec.z);
	}
	void RotateY(float angle)
	{
		Vector3 vec = *this;
		vec.x = cos(angle) * this->x + sin(angle) * this->z;
		vec.z = -sin(angle) * this->x + cos(angle) * this->z;
		*this = vec;
	}
	Vector3 GetRoteted(float angle)
	{
		Vector3 vec = *this;
		vec.RotateY(angle);
		return vec;
	}
	Vector3 GetInverseComponents()
	{
		return Vector3(1.0f/this->x, 1.0f/this->y, 1.0f/this->z);
	}

	inline float& operator[]( unsigned int i ) throw(const char*)
	{
		if(i > 2)
		{
			throw("index out of bounds");
		}
		return values[i];
	}
	inline const float& operator[]( unsigned int i ) const
	{
		if(i > 2)
		{
			throw("index out of bounds");
		}
		return values[i];
	}

#ifdef D3DVECTOR_DEFINED
	operator D3DXVECTOR3 () const { return D3DXVECTOR3(x,y,z); }
#endif
};


class Vector4
{
public:
	union
	{
		//the variable "values" and x,y,z and w share the same memory
		float values[4];
		struct
		{
			float x; //values[0]
			float y; //values[1]
			float z; //values[2]
			float w; //values[3]
		};
	};

	Vector4(float _x=0.0f, float _y=0.0f, float _z=0.0f, float _w=0.0f) : x(_x), y(_y), z(_z), w(_w)
	{

	}

	float GetLength() const
	{
		return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2) + pow(this->w, 2));
	}

	Vector4 operator+(const Vector4& v) const
	{
		return Vector4(this->x+v.x, this->y+v.y, this->z+v.z, this->w+v.w);
	}

	void normalize()
	{
		float length = this->GetLength();

		if(length > 0.0f)
		{
			this->x /= length;
			this->y /= length;
			this->z /= length;
			this->w /= length;
		}
	}

	inline float& operator[]( unsigned int i ) throw(const char*)
	{
		if(i > 3)
		{
			throw("index out of bounds");
		}
		return values[i];
	}
	inline const float& operator[]( unsigned int i ) const
	{
		if(i > 3)
		{
			throw("index out of bounds");
		}
		return values[i];
	}

#ifdef D3DVECTOR_DEFINED
	operator D3DXVECTOR4 () const { return D3DXVECTOR4(x,y,z,w); }
#endif
};

#pragma warning (pop)

#endif