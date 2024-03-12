#include "Worlds.h"

#include "Systems/Entity.h"
#include "Systems/Components.h"
#include "Math/Random.h"

namespace jm
{
	entity_id CreateSphereEntity(entity_registry& registry
		, f32 radius
		, f32 mass
		, math::vector3_f32 position
		, math::quaternion_f32 orientation
		, math::vector3_f32 velocity
		, math::vector3_f32 angular_velocity)
	{
		auto entity = registry.create();
		registry.emplace<spatial3_component>(entity, position, orientation);
		registry.emplace<linear_body3_component>(entity, velocity, mass);
		registry.emplace<rotational_body3_component>(entity, angular_velocity, math::get_sphere_inertia(mass, radius));
		registry.emplace<sphere_shape_component>(entity, radius);
		registry.emplace<collidable_component>(entity);
		return entity;
	}

	entity_id CreateBoxEntity(entity_registry& registry
		, math::vector3_f32 extents
		, f32 mass
		, math::vector3_f32 position
		, math::quaternion_f32 orientation
		, math::vector3_f32 velocity
		, math::vector3_f32 angular_velocity)
	{
		auto entity = registry.create();
		registry.emplace<spatial3_component>(entity, position, orientation);
		registry.emplace<linear_body3_component>(entity, velocity, mass);
		registry.emplace<rotational_body3_component>(entity, angular_velocity, math::get_box_inertia(mass, extents));
		registry.emplace<box_shape_component>(entity, extents);
		registry.emplace<collidable_component>(entity);
		return entity;
	}

	entity_id CreateSphereEntity(entity_registry& registry
		, f32 radius
		, f32 mass
		, math::vector3_f32 position
		, math::quaternion_f32 orientation)
	{
		return CreateSphereEntity(registry, radius, mass, position, orientation, 6.f * math::random::unit_ball<f32>(), math::random::unit_ball<f32>());
	}

	entity_id CreateSphereEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& orientation)
	{
		return CreateSphereEntity(registry, 1.f, 2.f, position, orientation);
	}

	entity_id CreateBoxEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& orientation, math::vector3_f32 const& extents)
	{
		return CreateBoxEntity(registry, extents, 2.f, position, orientation, 6.f * math::random::unit_ball<f32>(), math::random::unit_ball<f32>());
	}

	entity_id CreateBoxEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& orientation)
	{
		return CreateBoxEntity(registry, position, orientation, math::vector3_f32{1.f});
	}

	void CreateBasicWorld(entity_registry& registry)
	{
		CreateSphereEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
		CreateSphereEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
	}
}

