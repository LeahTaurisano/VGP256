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