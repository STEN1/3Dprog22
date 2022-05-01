#include "sMathTests.h"
#define SMATH_TEST
#include "sMath.h"

#include <iostream>
#include <iomanip>

namespace sMath
{
	constexpr float pi = 3.14159265358979323846f;

	template<typename T>
	std::ostream& operator << (std::ostream& out, const Vec3<T>& v)
	{
		out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
		return out;
	}

	template<typename T>
	std::ostream& operator << (std::ostream& out, const Mat4<T>& m)
	{
		std::cout << std::setprecision(1);
		for (size_t i = 0; i < 4; i++)
		{
			std::cout << "|\t";
			for (size_t j = 0; j < 4; j++)
			{
				std::cout << m(i, j) << "\t";
			}
			std::cout << "|" << std::endl;
		}
		return out;
	}



	void RunTests()
	{
		using Vec3 = Vec3<float>;
		using Mat4 = Mat4<float>;
		{
			std::cout << "Vector multiplication test\n";
			Vec3 v1{ 2.f, 2.f, 2.f };
			std::cout << "v1: " << v1 << std::endl;
			Vec3 v2{ 2.f, 0.f, 0.5f };
			std::cout << "v2: " << v2 << std::endl;
			Vec3 v3 = v1 * v2;
			std::cout << "v3 = v1 * v2: " << v3 << std::endl;
			v3 *= v1;
			std::cout << "v3 *= v1: " << v3 << std::endl;
		}
		{
			std::cout << "Vector division test\n";
			Vec3 v1{ 2.f, 2.f, 2.f };
			std::cout << "v1: " << v1 << std::endl;
			Vec3 v2{ 2.f, 1.f, 0.5f };
			std::cout << "v2: " << v2 << std::endl;
			Vec3 v3 = v1 / v2;
			std::cout << "v3 = v1 / v2: " << v3 << std::endl;
			v3 /= v1;
			std::cout << "v3 /= v1: " << v3 << std::endl;
		}
		{
			std::cout << "Testing vector member functions\n";
			Vec3 v1{ 2.f, 0.f, 0.f };
			Vec3 v2{ 0.f, 2.f, 0.f };
			Vec3 v3 = v1 + v2;
			std::cout << "v1: " << v1 << " size: " << v1.Size() << " sqared: " << v1.SizeSquared() << std::endl;
			std::cout << "v2: " << v2 << " v1.Dot(v2): " << v1.Dot(v2) << std::endl;
			std::cout << "Vec3::ZeroVec(): " << Vec3::ZeroVec() << std::endl;
			std::cout << "v3 = v1 + v2: " << v3 << std::endl;
			std::cout << "v3.Normalize().Size(): " << v3 << ' ' << v3.Normalize().Size()<< std::endl;
		}
		{
			std::cout << "Testing matrix multiplication\n";
			Mat4 m;
			std::cout << "m.SetToIdentity():\n";
			m.SetToIdentity();
			std::cout << m;
			std::cout << "m.Translate({ 1.f, 0.f, 0.f })\n";
			m.Translate({ 1.f, 0.f, 0.f });
			std::cout << m;
			std::cout << "m.rotate(90.f, { 0.f, 1.f, 0.f })\n";
			m.Rotate(90.f, { 0.f, 1.f, 0.f });
			std::cout << m;
			std::cout << "m.SetToIdentity().Rotate(pi / 2.f ,{ 0.f, 0.f, 1.f })\n";
			m.SetToIdentity().Rotate(pi / 2.f ,{ 0.f, 0.f, 1.f });
			std::cout << m;
			std::cout << "Vec3 v1 = m * Vec3{ 1.f, 1.f, 0.f }\n";
			Vec3 v1 = m * Vec3{ 1.f, 1.f, 0.f };
			std::cout << "v1: " << v1 << std::endl;
		}
	}
}
