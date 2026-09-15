#pragma once

#include <cmath>
#include <assert.h>
#include "../Core/Types.h"
#include "Vector.h"
#include "Matrix.h"

struct FVector4 {

	union
	{
		float V[4];
		struct
		{
			float X;
			float Y;
			float Z;
			float W;
		};
	};

	/* Constructor */
	FVector4() { X = 0.0f; Y = 0.0f; Z = 0.0f; W = 0.0f; } 
	FVector4(float x, float y, float z, float w) { X = x; Y = y; Z = z; W = w; }
	explicit FVector4(float f) { X = f; Y = f; Z = f; W = f; }
	FVector4(const FVector4& V1) { X = V1.X; Y = V1.Y; Z = V1.Z; W = V1.W; }
	FVector4(const FVector& V1, float w = 0.0f) { X = V1.X; Y = V1.Y; Z = V1.Z; W = w; }
	// ~FVector();

public:
	/* Public Functions */
	inline void Set(float x, float y, float z, float w) { X = x; Y = y; Z = z; W = w; }

	inline void Set(const FVector& V1, float w = 0.0f) { X = V1.X; Y = V1.Y; Z = V1.Z; W = w; }
	inline void Set(const FVector4& V1) { X = V1.X; Y = V1.Y; Z = V1.Z; W = V1.W; }
	inline float& Component(int32 Index) { return V[Index % 4];  }
	inline float Component(int32 Index) const { return V[Index % 4]; }
	inline float Size() { float sum = X * X + Y * Y + Z * Z + W * W; return sqrt(sum); }
	inline float Length() { float sum = X * X + Y * Y + Z * Z + W * W; return sqrt(sum); }
	inline float Dot(const FVector4& V1) const { return X * V1.X + Y * V1.Y + Z * V1.Z + W * V1.W; }

	inline FVector4 Normalize()
	{
		float size = Size();

		if (!FMath::IsNearlyZero(size))
		{
			X /= size;
			Y /= size;
			Z /= size;
			W /= size;
		}

		return*this;
	}
	inline FVector4 Cross(const FVector4& V1) const { return FVector4(Y * V1.Z - Z * V1.Y, Z * V1.X - X * V1.Z, X * V1.Y - Y * V1.X, 0); }
	inline FVector4 GetAbs()
	{
		float AbsX = X;
		float AbsY = Y;
		float AbsZ = Z;
		float AbsW = W;
		if (X < 0) AbsX = -X;
		if (Y < 0) AbsY = -Y;
		if (Z < 0) AbsZ = -Z;
		if (W < 0) AbsW = -W;
		return FVector4(AbsX, AbsY, AbsZ, AbsW);
	}


/* operator */

	inline FVector4 operator - () { return FVector4(-X, -Y, -Z, -W); }
	inline FVector4& operator = (const FVector4& V1) { X = V1.X; Y = V1.Y; Z = V1.Z; W = V1.W; return *this; }

	inline FVector4 operator - (const FVector4& V1) const { return FVector4(X - V1.X, Y - V1.Y, Z - V1.Z, W - V1.W);}
	inline FVector4& operator -= (const FVector4& V1) { X -= V1.X; Y -= V1.Y; Z -= V1.Z; W -= V1.W; return *this; }

	inline FVector4 operator + (const FVector4& V1) const {return FVector4(X + V1.X, Y + V1.Y, Z + V1.Z, W + V1.W); }

	inline FVector4& operator += (const FVector4& V1) { X += V1.X; Y += V1.Y; Z += V1.Z; W += V1.W; return *this; }

	inline FVector4 operator * (const FVector4& V1) const { return FVector4(X * V1.X, Y * V1.Y, Z * V1.Z, W * V1.W);}

	inline FVector4 operator * (const float& f) { return FVector4(X * f, Y * f, Z * f, W * f); }
	inline FVector4 operator * (const FMatrix& M) const { return M.TransformFVector4(*this); }

	inline FVector4& operator *= (const FVector4& V1) { X *= V1.X; Y *= V1.Y; Z *= V1.Z; W *= V1.W; return *this;}
	inline FVector4& operator *= (const float& f) { X *= f; Y *= f; Z *= f; W *= f; return *this;}

	inline FVector4 operator / (const FVector4& V1) const
	{
		assert(V1.X != 0 && V1.Y != 0 && V1.Z != 0 && V1.W != 0);
		return FVector4(X / V1.X, Y / V1.Y, Z / V1.Z, W / V1.W);
	}

	inline FVector4 operator / (const float& f) const { return FVector4(X / f, Y / f, Z / f, W / f); }

	inline FVector4& operator /= (const FVector4& V1) { X /= V1.X; Y /= V1.Y; Z /= V1.Z; W /= V1.W; return *this; }
	inline FVector4& operator /= (const float& f) { X /= f; Y /= f; Z /= f; W /= f; return *this; }

	inline FVector4 operator ^ (const FVector4& V1) const { return Cross(V1);}

	inline bool operator == (const FVector4& V1) const { return (X == V1.X) && (Y == V1.Y) && (Z == V1.Z) && (W == V1.W); }
	inline bool operator != (const FVector4& V1) const { return !(*this == V1); }

	inline float operator[] (int32 Index) const { return V[Index]; }
	inline float& operator[] (int32 Index) { return V[Index]; }


	inline static float DotProduct(const FVector4& V1, const FVector4& V2) { return V1.Dot(V2); }

	inline static FVector4 CrossProduct(const FVector4& V1, const FVector4& V2) { return V1.Cross(V2); }
	inline static float Distance(const FVector4& V1, const FVector4& V2)
	{
		float dX = V1.X - V2.X;
		float dY = V1.Y - V2.Y;
		float dZ = V1.Z - V2.Z;
		float dW = V1.W - V2.W;
		float sum = dX * dX + dY * dY + dZ * dZ + dW * dW;

		return sqrt(sum);
	}

	

};

/* Global Operator */
inline std::ostream& operator << (std::ostream& OS, const FVector4& V)
{
	OS << "(" << V.X << ", " << V.Y << ", " << V.Z << ", " << V.W << ")";
	return OS;
}