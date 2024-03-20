#pragma once

#include "Entity.h"
#include "MathTypes.h"

namespace jm
{
	void integrate(entity_registry& registry, f32 delta_time, math::vector3<f32> wind_force, math::vector3<f32> wall_boundaries, math::vector3<f32> wall_boundaries_max);
}