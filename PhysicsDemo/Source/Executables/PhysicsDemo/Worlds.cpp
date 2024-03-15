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
		return CreateSphereEntity(registry, 1.f, 2.f, position, orientation, pinned);
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
		for (int y = 0; y < 10; y += 2)
		{
			for (int x = 0; x < 10; x += 2)
			{
				bool pin = y == 8;
				entity_id mass = CreateSphereEntity(registry, { x, y, 0 }, math::random::unit_quaternion<f32>(), pin);

				if (x != 0)
				{
					CreateConstraintEntity(registry, 2.f, mass, spheres.back());
				}

				if (y != 0)
				{
					CreateConstraintEntity(registry, 2.f, mass, spheres[spheres.size() - 4]);
				}
				spheres.push_back(mass);
			}
		}

		/*entity_id massA = CreateSphereEntity(registry, {0, 0, 0}, math::random::unit_quaternion<f32>(), false);
		entity_id massB = CreateSphereEntity(registry, {0, 2, 0}, math::random::unit_quaternion<f32>(), false);
		entity_id massC = CreateSphereEntity(registry, {0, 4, 0}, math::random::unit_quaternion<f32>(), false);
		entity_id massD = CreateSphereEntity(registry, {0, 6, 0}, math::random::unit_quaternion<f32>(), false);
		entity_id massE = CreateSphereEntity(registry, {0, 8, 0}, math::random::unit_quaternion<f32>(), true);

		entity_id massF = CreateSphereEntity(registry, { 2, 0, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massG = CreateSphereEntity(registry, { 2, 2, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massH = CreateSphereEntity(registry, { 2, 4, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massI = CreateSphereEntity(registry, { 2, 6, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massJ = CreateSphereEntity(registry, { 2, 8, 0 }, math::random::unit_quaternion<f32>(), true);

		entity_id massK = CreateSphereEntity(registry, { 4, 0, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massL = CreateSphereEntity(registry, { 4, 2, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massM = CreateSphereEntity(registry, { 4, 4, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massN = CreateSphereEntity(registry, { 4, 6, 0 }, math::random::unit_quaternion<f32>(), false);
		entity_id massO = CreateSphereEntity(registry, { 4, 8, 0 }, math::random::unit_quaternion<f32>(), true);
		CreateConstraintEntity(registry, 2.f, massA, massB);
		CreateConstraintEntity(registry, 2.f, massB, massC);
		CreateConstraintEntity(registry, 2.f, massC, massD);
		CreateConstraintEntity(registry, 2.f, massD, massE);

		CreateConstraintEntity(registry, 2.f, massF, massG);
		CreateConstraintEntity(registry, 2.f, massG, massH);
		CreateConstraintEntity(registry, 2.f, massH, massI);
		CreateConstraintEntity(registry, 2.f, massI, massJ);

		CreateConstraintEntity(registry, 2.f, massA, massF);
		CreateConstraintEntity(registry, 2.f, massB, massG);
		CreateConstraintEntity(registry, 2.f, massC, massH);
		CreateConstraintEntity(registry, 2.f, massD, massI);

		CreateConstraintEntity(registry, 2.f, massK, massL);
		CreateConstraintEntity(registry, 2.f, massL, massM);
		CreateConstraintEntity(registry, 2.f, massM, massN);
		CreateConstraintEntity(registry, 2.f, massN, massO);

		CreateConstraintEntity(registry, 2.f, massF, massK);
		CreateConstraintEntity(registry, 2.f, massG, massL);
		CreateConstraintEntity(registry, 2.f, massH, massM);
		CreateConstraintEntity(registry, 2.f, massI, massN);*/
	}
}

