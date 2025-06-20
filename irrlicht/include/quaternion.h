// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __IRR_QUATERNION_H_INCLUDED__
#define __IRR_QUATERNION_H_INCLUDED__

#include "irrTypes.h"
#include "matrix4.h"

namespace irr
{
namespace core
{

//! Quaternion class. 
class quaternion
{
	public:

		//! Default Constructor
		quaternion();

		//! Constructor
		quaternion(f32 X, f32 Y, f32 Z, f32 W);

		//! Constructor which converts euler angles to a quaternion
		quaternion(f32 x, f32 y, f32 z);

		//! Constructor which converts a matrix to a quaternion
		quaternion(const matrix4& mat);

		//! equal operator
		bool operator==(const quaternion& other) const;

		//! assignment operator
		quaternion& operator=(const quaternion& other);

		//! matrix assignment operator
		quaternion& operator=(const matrix4& other);

		//! add operator
		quaternion operator+(const quaternion& other) const;

		//! multiplication operator
		quaternion operator*(const quaternion& other) const;

		//! multiplication operator
		quaternion operator*(f32 s) const;

		//! multiplication operator
		quaternion& operator*=(f32 s);

		//! multiplication operator
		quaternion& operator*=(const quaternion& other);

		//! calculates the dot product
		f32 getDotProduct(const quaternion& other) const;

		//! sets new quaternion 
		void set(f32 x, f32 y, f32 z, f32 w);

		//! sets new quaternion based on euler angles
		void set(f32 x, f32 y, f32 z);

		//! normalizes the quaternion
		quaternion& normalize();

		//! Creates a matrix from this quaternion
		matrix4 getMatrix() const;

		//! Inverts this quaternion
        void makeInverse();

		// Interpolates the quaternion between to quaternions based on time
		quaternion Slerp(quaternion q1, quaternion q2, f32 time);

		f32 X, Y, Z, W;
};


//! Default Constructor
inline quaternion::quaternion()
: X(0.0f), Y(0.0f), Z(0.0f), W(1.0f)
{
}

//! Constructor
inline quaternion::quaternion(f32 x, f32 y, f32 z, f32 w)
: X(x), Y(y), Z(z), W(w)
{
}

//! Constructor which converts euler angles to a quaternion
inline quaternion::quaternion(f32 x, f32 y, f32 z)
{
	set(x,y,z);
}


//! Constructor which converts a matrix to a quaternion
inline quaternion::quaternion(const matrix4& mat)
{
	(*this) = mat;
}


//! equal operator
inline bool quaternion::operator==(const quaternion& other) const
{
	if(X != other.X)
		return false;
	if(Y != other.Y)
		return false;
	if(Z != other.Z)
		return false;
	if(W != other.W)
		return false;

	return true;
}


//! assignment operator
inline quaternion& quaternion::operator=(const quaternion& other)
{
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	W = other.W;
	return *this;
}


//! matrix assignment operator
inline quaternion& quaternion::operator=(const matrix4& m)
{
	f32 diag = m(0,0) + m(1,1) + m(2,2) + 1;
	f32 scale = 0.0f;

	if( diag > 0.0f )
	{
		scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		// TODO: speed this up
		X = ( m(1,2) - m(2,1)) / scale;
		Y = ( m(2,0) - m(0,2)) / scale;
		Z = ( m(0,1) - m(1,0)) / scale;
		W = 0.25f * scale;
	}
	else
	{
		if ( m(0,0) > m(1,1) && m(0,0) > m(2,2))  
		{	
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			scale = sqrtf( 1.0f + m(0,0) - m(1,1) - m(2,2)) * 2.0f;

			// TODO: speed this up
			X = 0.25f * scale;
			Y = (m(0,1) + m(1,0)) / scale;
			Z = (m(2,0) + m(0,2)) / scale;
			W = (m(1,2) - m(2,1)) / scale;	
		} 
		else if ( m(1,1) > m(2,2)) 
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			scale = sqrtf( 1.0f + m(1,1) - m(0,0) - m(2,2)) * 2.0f;
			
			// TODO: speed this up
			X = (m(0,1) + m(1,0) ) / scale;
			Y = 0.25f * scale;
			Z = (m(1,2) + m(2,1) ) / scale;
			W = (m(2,0) - m(0,2) ) / scale;
		} 
		else 
		{	
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			scale  = sqrtf( 1.0f + m(2,2) - m(0,0) - m(1,1)) * 2.0f;

			// TODO: speed this up
			X = (m(2,0) + m(0,2)) / scale;
			Y = (m(1,2) + m(2,1)) / scale;
			Z = 0.25f * scale;
			W = (m(0,1) - m(1,0)) / scale;
		}
	}

	normalize();
	return *this;
}


//! multiplication operator
inline quaternion quaternion::operator*(const quaternion& other) const
{
	quaternion tmp;

	tmp.W = (other.W * W) - (other.X * X) - (other.Y * Y) - (other.Z * Z);
	tmp.X = (other.W * X) + (other.X * W) + (other.Y * Z) - (other.Z * Y);
	tmp.Y = (other.W * Y) + (other.Y * W) + (other.Z * X) - (other.X * Z);
	tmp.Z = (other.W * Z) + (other.Z * W) + (other.X * Y) - (other.Y * X);

	return tmp;
}


//! multiplication operator
inline quaternion quaternion::operator*(f32 s) const
{
	return quaternion(s*X, s*Y, s*Z, s*W);
}

//! multiplication operator
inline quaternion& quaternion::operator*=(f32 s)
{
	X *= s; Y*=s; Z*=s; W*=s;
	return *this;
}

//! multiplication operator
inline quaternion& quaternion::operator*=(const quaternion& other)
{
	*this = other * (*this);
	return *this;
}

//! add operator
inline quaternion quaternion::operator+(const quaternion& b) const
{
	return quaternion(X+b.X, Y+b.Y, Z+b.Z, W+b.W);
}


//! Creates a matrix from this quaternion
inline matrix4 quaternion::getMatrix() const
{
	core::matrix4 m;

	m(0,0) = 1.0f - 2.0f*Y*Y - 2.0f*Z*Z; 
	m(0,1) = 2.0f*X*Y + 2.0f*Z*W; 
	m(0,2) = 2.0f*X*Z - 2.0f*Y*W; 
	m(0,3) = 0.0f;

	m(1,0) = 2.0f*X*Y - 2.0f*Z*W; 
	m(1,1) = 1.0f - 2.0f*X*X - 2.0f*Z*Z; 
	m(1,2) = 2.0f*Z*Y + 2.0f*X*W; 
	m(1,3) = 0.0f;

	m(2,0) = 2.0f*X*Z + 2.0f*Y*W; 
	m(2,1) = 2.0f*Z*Y - 2.0f*X*W; 
	m(2,2) = 1.0f - 2.0f*X*X - 2.0f*Y*Y; 
	m(2,3) = 0.0f;

	m(3,0) = 0.0f; 
	m(3,1) = 0.0f; 
	m(3,2) = 0.0f; 
	m(3,3) = 1.0f;

	return m;
}


//! Inverts this quaternion
inline void quaternion::makeInverse()
{
	X = -X; Y = -Y; Z = -Z;
}

//! sets new quaternion 
inline void quaternion::set(f32 x, f32 y, f32 z, f32 w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

//! sets new quaternion based on euler angles
inline void quaternion::set(f32 x, f32 y, f32 z)
{
	f32 angle;

	angle = x * 0.5f;
	f32 sr = (f32)sin(angle);
	f32 cr = (f32)cos(angle);

	angle = y * 0.5f;
    f32 sp = (f32)sin(angle);
	f32 cp = (f32)cos(angle);

	angle = z * 0.5f;
	f32 sy = (f32)sin(angle);
	f32 cy = (f32)cos(angle);

	f32 cpcy = cp * cy;
	f32 spcy = sp * cy;
	f32 cpsy = cp * sy;
	f32 spsy = cp * sy;

	X = sr * cpcy - cr * spsy;
	Y = cr * spcy + sr * cpsy;
	Z = cr * cpsy - sr * spcy;
	W = cr * cpcy + sr * spsy;

	normalize();
}

//! normalizes the quaternion
inline quaternion& quaternion::normalize()
{
	f32 n = X*X + Y*Y + Z*Z + W*W;

	if (n == 1)
		return *this; 

	n = 1.0f / sqrtf(n);
	X *= n;
	Y *= n;
	Z *= n;
	W *= n;

	return *this;
}


// Interpolates the quaternion between to quaternions based on time
inline quaternion quaternion::Slerp(quaternion q1, quaternion q2, f32 time)
{
    f32 angle = q1.getDotProduct(q2);

    if (angle < 0.0f) 
    {
		q1 *= -1.0f;
		angle *= -1.0f;
    }

	f32 scale;
	f32 invscale;

    if ((angle + 1.0f) > 0.05f) 
    {
		if ((1.0f - angle) >= 0.05f)  // spherical interpolation
		{
			f32 theta = (f32)acos(angle);
			f32 invsintheta = 1.0f / (f32)sin(theta);
			scale = (f32)sin(theta * (1.0f-time)) * invsintheta;
			invscale = (f32)sin(theta * time) * invsintheta;
		}
		else // linear interploation
		{
			scale = 1.0f - time;
			invscale = time;
		}
    }
    else 
    {
        q2 = quaternion(-q1.Y, q1.X, -q1.W, q1.Z);
        scale = (f32)sin(PI * (0.5f - time));
        invscale = (f32)sin(PI * time);
    }

	*this = (q1*scale) + (q2*invscale);
	return *this;
}


//! calculates the dot product
inline f32 quaternion::getDotProduct(const quaternion& q2) const
{
	return (X * q2.X) + (Y * q2.Y) + (Z * q2.Z) + (W * q2.W);
}

} // end namespace core
} // end namespace irr

#endif 

