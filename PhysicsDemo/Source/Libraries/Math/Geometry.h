#pragma once

#include "MathTypes.h"

namespace jm::math
{
	template <typename T>
	struct sphere3
	{
		vector3<T> centre{};
		T radius{};
	};

	template <typename T>
	struct box3
	{
		vector3<T> position{}; //center
		vector3<T> extents{}; //half length along each axis
		math::matrix33<T> axes{}; //orthonormal
	};

	template <typename T>
	struct ray3
	{
		vector3<T> origin{};
		vector3<T> direction{}; //assumes normalized
	};

	template <typename T>
	bool intersects(sphere3<T> const& sphere, ray3<T> const& ray, T& t)
	{
		//t^2 + 2bt + c = 0
		const vector3<T> offset = ray.origin - sphere.centre;
		const T b = dot(offset, ray.direction);
		const T c = dot(offset, offset) - sphere.radius * sphere.radius;

		if (c > T(0) && b > T(0))
		{
			//ray origin outside the sphere AND ray pointing away from sphere (obtuse angle)
			return false;
		}

		const T D = b * b - c;
		if (D < T(0))
		{
			//negative determinant, misses sphere
			return false;
		}

		t = -b - std::sqrt(D);
		if (t < T(0))
		{
			t = T(0); //ray started inside sphere
		}

		return true;
	}

	template <typename T>
	bool intersects(sphere3<T> const& a, sphere3<T> const& b)
	{
		const vector3<T> center_displacement = a.centre - b.centre;
		return dot(center_displacement, center_displacement) < a.radius + b.radius + math::epsilon<T>();
	}

	template <typename T>
	bool intersects(vector3<T> const& a, box3<T> const& b)
	{
		vector3<T> box_local_point = a - b.position;
		box_local_point = glm::transpose(b.axes) * box_local_point;

		return std::abs(box_local_point.x) < b.extents.x + math::epsilon<T>() &&
			std::abs(box_local_point.y) < b.extents.y + math::epsilon<T>() &&
			std::abs(box_local_point.z) < b.extents.z + math::epsilon<T>();

	}

	template <typename T>
	bool intersects(sphere3<T> const& a, box3<T> const& b)
	{
		vector3<T> box_local_point = a.centre - b.position;
		box_local_point = glm::transpose(b.axes) * box_local_point;

		return std::abs(box_local_point.x) < b.extents.x + math::epsilon<T>() &&
			std::abs(box_local_point.y) < b.extents.y + a.radius + math::epsilon<T>() &&
			std::abs(box_local_point.z) < b.extents.z + a.radius + math::epsilon<T>();
	}
}