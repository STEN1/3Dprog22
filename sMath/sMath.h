#pragma once
#ifndef SMATH_TEST
	#include <QMatrix4x4>
#endif // !SMATH_TEST
#include <math.h>
namespace sMath
{
	template<typename T>
	struct Vec3
	{
		T x{};
		T y{};
		T z{};
		// Math with other vectors
		Vec3 operator+(const Vec3& v) const
		{
			return { x + v.x, y + v.y, z + v.z };
		}
		Vec3 operator-(const Vec3& v) const
		{
			return { x - v.x, y - v.y, z - v.z };
		}
		Vec3 operator*(const Vec3& v) const
		{
			return { x * v.x, y * v.y, z * v.z };
		}
		Vec3 operator/(const Vec3& v) const
		{
			return { x / v.x, y / v.y, z / v.z };
		}
		Vec3& operator*=(const Vec3& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}
		Vec3& operator/=(const Vec3& v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}
		// Math with constants
		Vec3 operator*(T s) const
		{
			return { x * s, y * s, z * s };
		}
		Vec3 operator/(T s) const
		{
			return { x / s, y / s, z / s };
		}
		Vec3& operator/=(T s)
		{
			x /= s;
			y /= s;
			z /= s;
			return *this;
		}
		Vec3& operator*=(T s)
		{
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		// Bool operations
		bool operator==(const Vec3& v) const
		{
			return x == v.x && y == v.y && z == v.z;
		}
		bool operator!=(const Vec3& v) const
		{
			return x != v.x || y != v.y || z != v.z;
		}
		T Size() const
		{
			return sqrt(x * x + y * y + z * z);
		}
		T SizeSquared() const
		{
			return x * x + y * y + z * z;
		}
		T Dot(const Vec3& b) const
		{
			return { x * b.x + y * b.y + z * b.z };
		}
		Vec3& Normalize()
		{
			return *this /= Size();
		}
		static Vec3 ZeroVec()
		{
			return { 0, 0, 0 };
		}
	};

	template<typename T>
	class Quat
	{
	public:

	};

	template<typename T>
	class Mat4
	{
	public:
		Mat4() = default;
#ifndef SMATH_TEST
		Mat4(const QMatrix4x4& qMat)
		{ // QMatrix4x4 has column major data internally, but we dont care because it has row major access from operator().
			mat[0][0] = qMat(0, 0);	mat[0][1] = qMat(0, 1);	mat[0][2] = qMat(0, 2);	mat[0][3] = qMat(0, 3);
			mat[1][0] = qMat(1, 0);	mat[1][1] = qMat(1, 1);	mat[1][2] = qMat(1, 2);	mat[1][3] = qMat(1, 3);
			mat[2][0] = qMat(2, 0);	mat[2][1] = qMat(2, 1);	mat[2][2] = qMat(2, 2);	mat[2][3] = qMat(2, 3);
			mat[3][0] = qMat(3, 0);	mat[3][1] = qMat(3, 1);	mat[3][2] = qMat(3, 2);	mat[3][3] = qMat(3, 3);
		}
#endif // !SMATH_TEST
		Mat4& SetToIdentity()
		{
			mat[0][0] = T(1.0);	mat[0][1] = T(0.0);	mat[0][2] = T(0.0);	mat[0][3] = T(0.0);
			mat[1][0] = T(0.0);	mat[1][1] = T(1.0);	mat[1][2] = T(0.0);	mat[1][3] = T(0.0);
			mat[2][0] = T(0.0);	mat[2][1] = T(0.0);	mat[2][2] = T(1.0);	mat[2][3] = T(0.0);
			mat[3][0] = T(0.0);	mat[3][1] = T(0.0);	mat[3][2] = T(0.0);	mat[3][3] = T(1.0);
			return *this;
		}
		Mat4& Translate(Vec3<T> deltaPos)
		{
			mat[0][3] += deltaPos.x;
			mat[1][3] += deltaPos.y;
			mat[2][3] += deltaPos.z;
			return *this;
		}
		Mat4& Rotate(T angle, Vec3<T> axis)
		{
			axis.Normalize();

			// create rotation matrix. https://learnopengl.com/Getting-started/Transformations
			Mat4 rot;
			rot(0, 0) = cos(angle) + axis.x * axis.x * (T(1) - cos(angle));
			rot(0, 1) = axis.x * axis.y * (T(1) - cos(angle)) - axis.z * sin(angle);
			rot(0, 2) = axis.x * axis.z * (T(1) - cos(angle)) + axis.y * sin(angle);
			rot(0, 3) = T(0);

			rot(1, 0) = axis.y * axis.x * (T(1) - cos(angle)) + axis.z * sin(angle);
			rot(1, 1) = cos(angle) + axis.y * axis.y * (T(1) - cos(angle));
			rot(1, 2) = axis.y * axis.z * (T(1) - cos(angle)) - axis.x * sin(angle);
			rot(1, 3) = T(0);

			rot(2, 0) = axis.z * axis.x * (T(1) - cos(angle)) - axis.y * sin(angle);
			rot(2, 1) = axis.z * axis.y * (T(1) - cos(angle)) + axis.x * sin(angle);
			rot(2, 2) = cos(angle) + axis.z * axis.z * (T(1) - cos(angle));
			rot(2, 3) = T(0);

			rot(3, 0) = T(0);
			rot(3, 1) = T(0);
			rot(3, 2) = T(0);
			rot(3, 3) = T(1);

			*this = *this * rot;

			return *this;
		}
		Mat4& Scale(const Vec3<T>& vScale)
		{

			*this;
		}
		void LookAt(const Vec3<T>& eye, const Vec3<T> center, const Vec3<T> up)
		{

		}

		Mat4 operator*(const Mat4& b)
		{
			Mat4 m{};
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					for (size_t k = 0; k < 4; k++)
					{
						m(i, j) += mat[i][k] * b(k, j);
					}
				}
			}
			return m;
		}



		T& operator()(const size_t& row, const size_t& col)
		{ 
			return mat[row][col];
		}
		const T& operator()(const size_t& row, const size_t& col) const
		{
			return mat[row][col];
		}
		//[][] will return copy with this.
		//auto operator[](size_t i) -> T(&)[4]
		//{
		//	return mat[i];
		//}

		
	private:
		T mat[4][4];
	};

	template<typename T>
	Vec3<T> operator*(const Mat4<T>& m, const Vec3<T>& b)
	{
		Vec3<T> v{};

		v.x = m(0, 0) * b.x + m(0, 1) * b.y + m(0, 2) * b.z + m(0, 3) * T(1);
		v.y = m(1, 0) * b.x + m(1, 1) * b.y + m(1, 2) * b.z + m(1, 3) * T(1);
		v.z = m(2, 0) * b.x + m(2, 1) * b.y + m(2, 2) * b.z + m(2, 3) * T(1);

		return v;
	}
}