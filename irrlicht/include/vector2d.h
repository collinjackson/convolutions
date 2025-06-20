// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __IRR_POINT_2D_H_INCLUDED__
#define __IRR_POINT_2D_H_INCLUDED__

#include "irrTypes.h"

namespace irr
{
namespace core
{


//! 2d vector template class with lots of operators and methods.
template <class T>
class vector2d
{
public:

	vector2d(): X(0), Y(0) {};
	vector2d(T nx, T ny) : X(nx), Y(ny) {};
	vector2d(const vector2d<T>& other)	:X(other.X), Y(other.Y) {};

	// operators

	vector2d<T>& operator=(const vector2d<T>& other)	{ X = other.X; Y = other.Y; return *this; }

	vector2d<T> operator+(const vector2d<T>& other) const { return vector2d<T>(X + other.X, Y + other.Y);	}
	vector2d<T>& operator+=(const vector2d<T>& other)	{ X+=other.X; Y+=other.Y; return *this; }

	vector2d<T> operator-(const vector2d<T>& other) const { return vector2d<T>(X - other.X, Y - other.Y);	}
	vector2d<T>& operator-=(const vector2d<T>& other)	{ X-=other.X; Y-=other.Y; return *this; }

	vector2d<T> operator*(const vector2d<T>& other) const { return vector2d<T>(X * other.X, Y * other.Y);	}
	vector2d<T>& operator*=(const vector2d<T>& other)	{ X*=other.X; Y*=other.Y; return *this; }
	vector2d<T> operator*(const T v) const { return vector2d<T>(X * v, Y * v);	}
	vector2d<T>& operator*=(const T v) { X*=v; Y*=v; return *this; }

	vector2d<T> operator/(const vector2d<T>& other) const { return vector2d<T>(X / other.X, Y / other.Y);	}
	vector2d<T>& operator/=(const vector2d<T>& other)	{ X/=other.X; Y/=other.Y; return *this; }
	vector2d<T> operator/(const T v) const { return vector2d<T>(X / v, Y / v);	}
	vector2d<T>& operator/=(const T v) { X/=v; Y/=v; return *this; }

	bool operator==(const vector2d<T>& other) const { return other.X==X && other.Y==Y; }
	bool operator!=(const vector2d<T>& other) const { return other.X!=X || other.Y!=Y; }

	// functions

	void set(const T& nx, const T& ny) {X=nx; Y=ny; }
	void set(const vector2d<T>& p) { X=p.X; Y=p.Y;}

	//! Returns the length of the vector
	//! \return Returns the length of the vector.
	f64 getLength() const { return sqrt(X*X + Y*Y); }

	//! Returns the dot product of this vector with an other.
	T dotProduct(const vector2d<T>& other) const
	{
		return X*other.X + Y*other.Y;
	}

	//! Returns distance from an other point. Here, the vector is interpreted as
	//! point in 2 dimensional space.
	f64 getDistanceFrom(const vector2d<T>& other) const
	{
		f64 vx = X - other.X; f64 vy = Y - other.Y;
		return sqrt(vx*vx + vy*vy);
	}

	//! rotates the point around a center by an amount of degrees.
	void rotateBy(f64 degrees, const vector2d<T>& center)
	{
		degrees *=gradPI2;
		T cs = (T)cos(degrees);
		T sn = (T)sin(degrees);

		X -= center.X;
		Y -= center.Y;

		set(X*cs - Y*sn, X*sn + Y*cs);

		X += center.X;
		Y += center.Y;
	}

	//! normalizes the vector.
	vector2d<T>& normalize()
	{
		T len = (T)(1.0 / getLength());
		X *= len;
		Y *= len;
		return *this;
	}

	//! Calculates the angle of this vector in grad.
	//! \return Returns a value between 0 and 360.
	inline f64 getAngle() const
	{
		if (Y == 0.0)
			return X > 0.0 ? 90.0 : 180.0;

		f64 tmp = Y / sqrt(X*X + Y*Y);
		tmp = atan(sqrt(1 - tmp*tmp) / tmp) * GRAD_PI;

		if (X>0.0 && Y>0.0)
			return tmp + 270;
		else
		if (X>0.0 && Y<0.0)
			return tmp + 90;
		else
		if (X<0.0 && Y<0.0)
			return 90 - tmp;
		else
		if (X<0.0 && Y>0.0)
			return 270 - tmp;

		return tmp;
	}

	//! Calculates the angle between this vector and another one in grad.
	//! \return Returns a value between 0 and 90.
	inline f64 getAngleWith(const vector2d<T>& b) const
	{
		f64 tmp = X*b.X + Y*b.Y;

		if (tmp == 0.0)
			return 90.0;

		tmp = tmp / sqrt((X*X + Y*Y) * (b.X*b.X + b.Y*b.Y));
		if (tmp < 0.0) tmp = -tmp;

		return atan(sqrt(1 - tmp*tmp) / tmp) * GRAD_PI;
	}

	// member variables
	T X, Y;
};

	//! Typedef for f32 2d vector.
	typedef vector2d<f32> vector2df;
	//! Typedef for integer 2d vector.
	typedef vector2d<s32> vector2di;

} // end namespace core
} // end namespace irr

#endif

