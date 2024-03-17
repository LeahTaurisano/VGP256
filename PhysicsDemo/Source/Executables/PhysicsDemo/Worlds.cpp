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
		, math::vector3_f32 angular_velocity
		, bool pinned)
	{
		auto entity = registry.create();
		registry.emplace<spatial3_component>(entity, position, orientation);
		registry.emplace<linear_body3_component>(entity, velocity, mass);
		registry.emplace<rotational_body3_component>(entity, angular_velocity, math::get_sphere_inertia(mass, radius));
		registry.emplace<sphere_shape_component>(entity, radius);
		registry.emplace<collidable_component>(entity);
		registry.emplace<pinned_component>(entity, pinned);
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
		, math::quaternion_f32 orientation
		, bool pinned)
	{
		return CreateSphereEntity(registry, radius, mass, position, orientation, 6.f * math::random::unit_ball<f32>(), math::random::unit_ball<f32>(), pinned);
	}

	entity_id CreateSphereEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& orientation, bool pinned)
	{
		return CreateSphereEntity(registry, 0.5f, 2.f, position, orientation, pinned);
	}

	entity_id CreateBoxEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& orientation, math::vector3_f32 const& extents)
	{
		return CreateBoxEntity(registry, extents, 2.f, position, orientation, 6.f * math::random::unit_ball<f32>(), math::random::unit_ball<f32>());
	}

	entity_id CreateBoxEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& orientation)
	{
		return CreateBoxEntity(registry, position, orientation, math::vector3_f32{1.f});
	}

	entity_id CreateConstraintEntity(entity_registry& registry
		, f32 linkDistance
		, entity_id massA
		, entity_id massB)
	{
		auto entity = registry.create();
		registry.emplace<constraint_component_rigid>(entity, linkDistance, massA, massB);
		return entity;
	}

	void CreateBasicWorld(entity_registry& registry)
	{
		std::vector<entity_id> spheres;
		//for (int y = 0; y < 10; ++y)
		//{
		//	for (int x = 0; x < 10; ++x)
		//	{
		//		bool pin = y == 0;
		//		entity_id mass = CreateSphereEntity(registry, { x + 5, y + 10, 0 }, math::random::unit_quaternion<f32>(), pin);

		//		if (x != 0)
		//		{
		//			CreateConstraintEntity(registry, 1.f, mass, spheres.back());
		//		}

		//		if (y != 0)
		//		{
		//			CreateConstraintEntity(registry, 1.f, spheres[spheres.size() - 10], mass);
		//		}
		//		spheres.push_back(mass);
		//	}
		//}
		//spheres.clear();
		for (int z = 0; z < 10; ++z)
		{
			for (int x = 0; x < 10; ++x)
			{
				bool pin = x == 0;
				entity_id mass = CreateSphereEntity(registry, { x, 8, z }, math::random::unit_quaternion<f32>(), pin);

				if (x != 0)
				{
					CreateConstraintEntity(registry, 1.f, mass, spheres.back());
				}

				if (z != 0)
				{
					CreateConstraintEntity(registry, 1.f, spheres[spheres.size() - 10], mass);
				}
				spheres.push_back(mass);
			}
		}
		spheres.clear();
		for (int y = 0; y < 20; ++y)
		{
			bool pin = y == 19;
			entity_id mass = CreateSphereEntity(registry, { -5, y + 2, -5 }, math::random::unit_quaternion<f32>(), pin);
			
			if (y != 0)
			{
				CreateConstraintEntity(registry, 1.f, mass, spheres.back());
			}
			spheres.push_back(mass);
		}
		
		/*entity_id massHead = CreateSphereEntity(registry, 0.5f, 2.f, { 5, 5, -5 }, math::random::unit_quaternion<f32>(), false);
		entity_id massPelvis = CreateSphereEntity(registry, 0.1f, 2.f, { 5, 3, -5 }, math::random::unit_quaternion<f32>(), false);
		CreateConstraintEntity(registry, 2.f, massHead, massPelvis);
		entity_id massLeftElbow = CreateSphereEntity(registry, 0.5f, 2.f, { 5, 5, -5 }, math::random::unit_quaternion<f32>(), false);*/

	}
}

