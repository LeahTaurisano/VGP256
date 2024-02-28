#pragma once

#include "Entity.h"
#include "Math/Geometry.h"
#include <optional>

namespace jm
{
	struct sphere_collider
	{
		entity_id entity;
		math::sphere3<f32> sphere;
	};

	struct box_collider
	{
		entity_id entity;
		math::box3<f32> box;
	};

	struct collider_set
	{
		std::vector<sphere_collider> spheres{};
		std::vector<box_collider> boxes{};
	};

	struct entity_offset
	{
		entity_id entity = null_entity_id;
		math::vector3_f32 offset{}; //relative to the collider origin
	};

	using entity_pick = std::optional<entity_offset>;

	collider_set build_colliders(entity_registry& registry);
	void resolve_collisions(entity_registry& registry, collider_set const& colliders);
	entity_pick ray_cast(collider_set const& colliders, math::ray3<f32> const& ray);
}