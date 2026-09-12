#pragma once

#include <cmath>
#include <assert.h>
#include "../Core/Types.h"
#include "EngineMath.h"

struct FVector {

	union
	{
		float V[3];
		struct
		{
			float X;
			float Y;
			float Z;
		};
	};

public:
	/* Constructor */
	FVector() { X = 0; Y = 0; Z = 0; }
	FVector(float InX, float InY, float InZ) { X = InX; Y = InY; Z = InZ; }	
	explicit FVector(float f) { X = f; Y = f; Z = f; }
	FVector(const FVector& V1) { X = V1.X; Y = V1.Y; Z = V1.Z; }
	// ~FVector();

public:
	/* Public Functions */
	inline void Set(float InX, float InY, float InZ) { X = InX; Y = InY; Z = InZ;}
	inline float Size() const { float sum = X * X + Y * Y + Z * Z; return sqrt(sum);}
	inline float Length() const { float sum = X * X + Y * Y + Z * Z; return sqrt(sum);}
	inline float Dot(const FVector& V1) const { return X * V1.X + Y * V1.Y + Z * V1.Z; }

	inline float& Component(int32 Index) { return V[Index % 3]; }
	inline float Component(int32 Index) const { return V[Index % 3];}

	inline FVector Add(const FVector& V1) const { return FVector(X + V1.X, Y + V1.Y, Z + V1.Z); }
	inline FVector Subtract(const FVector& V1) const { return FVector(X - V1.X, Y - V1.Y, Z - V1.Z); }

	inline FVector Cross(const FVector& V1) const { return FVector(Y * V1.Z - Z * V1.Y, Z * V1.X - X * V1.Z, X * V1.Y - Y * V1.X); }

	inline FVector GetAbs() const
	{
		float AbsX = X;
		float AbsY = Y;
		float AbsZ = Z;
		if (X < 0) AbsX = -X;
		if (Y < 0) AbsY = -Y;
		if (Z < 0) AbsZ = -Z;
		return FVector(AbsX, AbsY, AbsZ);
	}

	inline FVector Normalize() const
	{
		float size = Size();

		if (!FMath::IsNearlyZero(size))
		{
			return FVector(X / size, Y / size, Z / size);
		}

		return *this;
	}

/* operator */

	inline FVector operator - () { return FVector(-X, -Y, -Z); }
	inline FVector operator - () const { return FVector(-X, -Y, -Z); }
	inline FVector& operator = (const FVector& V1) { X = V1.X; Y = V1.Y; Z = V1.Z; return *this; }

	inline FVector operator - (const FVector& V1) const { return FVector(X - V1.X, Y - V1.Y, Z - V1.Z); }
	inline FVector& operator -= (const FVector& V1) { X -= V1.X; Y -= V1.Y; Z -= V1.Z; return *this; }

	inline FVector operator + (const FVector& V1) const { return FVector(X + V1.X, Y + V1.Y, Z + V1.Z); }
	inline FVector& operator += (const FVector& V1) { X += V1.X; Y += V1.Y; Z += V1.Z; return *this; }

	inline FVector operator * (const FVector& V1) const { return FVector(X * V1.X, Y * V1.Y, Z * V1.Z); }

	inline FVector operator * (const float& f) const { return FVector(X * f, Y * f, Z * f); }


	inline FVector& operator *= (const FVector& V1) { X *= V1.X; Y *= V1.Y; Z *= V1.Z; return *this; }
	inline FVector& operator *= (const float& f) { X *= f; Y *= f; Z *= f; return *this; }


	inline FVector operator / (const FVector& V1) const
	{
		assert(V1.X != 0 || V1.Y != 0 || V1.Z != 0);
		return FVector(X / V1.X, Y / V1.Y, Z / V1.Z);
	}

	inline FVector operator / (const float& f) const
	{
		return FVector(X / f, Y / f, Z / f);
	}

	inline FVector& operator /= (const FVector& V1)
	{
		const float InvX = 1.0f / V1.X;
		const float InvY = 1.0f / V1.Y;
		const float InvZ = 1.0f / V1.Z;

		X *= InvX;
		Y *= InvY;
		Z *= InvZ;

		return *this;
	}

	inline FVector& operator /= (const float& f)
	{
		const float InvF = 1.0f / f;
		X *= InvF;
		Y *= InvF;
		Z *= InvF;

		return *this;
	}
	
	// 스트림 출력 연산자와 함께 사용할 시 괄호로 묶을 것
	inline FVector operator ^ (const FVector& V1) const { return this->Cross(V1); }

	inline bool operator == (const FVector& V1) const { return (X == V1.X) && (Y == V1.Y) && (Z == V1.Z); }
	inline bool operator != (const FVector& V1) const { return !(*this == V1); }

	inline float operator[] (int32 Index) const { return V[Index]; }

	inline float& operator[] (int32 Index) { return V[Index]; }


	/* Static */
	inline static float DotProduct(const FVector& V1, const FVector& V2) { return V1.Dot(V2); }
	inline static FVector CrossProduct(const FVector& V1, const FVector& V2) { return V1.Cross(V2); }
	inline static float Distance(const FVector& V1, const FVector& V2)
	{
		float dX = V1.X - V2.X;
		float dY = V1.Y - V2.Y;
		float dZ = V1.Z - V2.Z;
		float sum = dX * dX + dY * dY + dZ * dZ;

		return sqrt(sum);
	}

};

/* Global Operator */
inline std::ostream& operator<<(std::ostream& OS, const FVector& V)
{
	OS << "(" << V.X << ", " << V.Y << ", " << V.Z << ")";
	return OS;
}

/* constants */
inline static const FVector BackwardVector = FVector(-1.0f, 0.0f, 0.0f);
inline static const FVector DownVector = FVector(0.0f, 0.0f, -1.0f);
inline static const FVector ForwardVector = FVector(1.0f, 0.0f, 0.0f);
inline static const FVector LeftVector = FVector(0.0f, -1.0f, 0.0f);
inline static const FVector OneVector = FVector(1.0f, 1.0f, 1.0f);
inline static const FVector RightVector = FVector(0.0f, 1.0f, 0.0f);
inline static const FVector UpVector = FVector(0.0f, 0.0f, 1.0f);
inline static const FVector XAxisVector = FVector(1.0f, 0.0f, 0.0f);
inline static const FVector YAxisVector = FVector(0.0f, 1.0f, 0.0f);
inline static const FVector ZAxisVector = FVector(0.0f, 0.0f, 1.0f);
inline static const FVector ZeroVector = FVector(0.0f, 0.0f, 0.0f);