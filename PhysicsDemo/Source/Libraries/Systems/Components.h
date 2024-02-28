#pragma once

#include "Math/Physics.h"

namespace jm
{
	using spatial2_component = math::rigid_motion2<f32>;

	using spatial3_component = math::rigid_motion3<f32>;

	struct disk_shape_component
	{
		f32 radius{};
	};

	struct sphere_shape_component
	{
		f32 radius{};
	};

	struct rectangle_shape_component
	{
		math::vector2_f32 extents{};
	};

	struct box_shape_component
	{
		math::vector3_f32 extents{};
	};

	struct collidable_component
	{
	};

	using linear_body2_component = math::linear_body2<f32>;
	using linear_body3_component = math::linear_body3<f32>;

	using rotational_body2_component = math::rotational_body2<f32>;
	using rotational_body3_component = math::rotational_body3<f32>;
}