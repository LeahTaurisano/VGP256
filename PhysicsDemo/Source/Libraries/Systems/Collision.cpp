#include "Collision.h"
#include "Components.h"


namespace jm
{
	collider_set build_colliders(entity_registry& registry)
	{
		auto shape_entity_view = registry.view<const shape_component, const spatial3_component>();

		std::vector<SphereCollider> spheres;
		std::vector<BoxCollider> boxes;
		for (auto&& [entity, shape, spatial] : shape_entity_view.each())
		{
			switch (shape)
			{
			case shape_component::sphere:
				spheres.push_back({ entity, math::sphere3{ spatial.position, 1.0f } }); //assume unit radius spheres
			default:
				boxes.push_back({ entity,
					math::box3{spatial.position,
					math::vector3_f32{1.0f},
					math::quat_to_mat(spatial.rotation)} });
				break;
			}
		}

		return { spheres, boxes };
	}

	entity_pick ray_cast(collider_set const& colliders, math::ray3<f32> const& ray)
	{
		f32 t_min = std::numeric_limits<f32>::infinity();
		entity_id entity_closest = null_entity_id;
		math::vector3_f32 offset{};
		for (SphereCollider const& collider : colliders.spheres)
		{
			f32 t_intersect = std::numeric_limits<f32>::infinity();
			if (math::intersects(collider.sphere, ray, t_intersect) &&
				t_intersect < t_min)
			{
				entity_closest = collider.entity;
				t_min = t_intersect;
				offset = (ray.origin + t_intersect * ray.direction) - collider.sphere.centre;
			}
		}
		/*for (BoxCollider const& box : colliders.boxes)
		{
			
		}*/

		if (entity_closest != null_entity_id)
		{
			return entity_offset{ entity_closest, offset };
		}
		return std::nullopt;
	}

	void resolve_collisions(entity_registry& registry, collider_set const& colliders)
	{
		registry;
		//check for collisions
		for (size_t idx = 0; idx < colliders.spheres.size(); ++idx)
		{
			auto& b = colliders.spheres[idx];
			for (size_t jdx = idx + 1; jdx < colliders.spheres.size(); ++jdx)
			{
				auto& a = colliders.spheres[jdx];
				if (math::intersects(a.sphere, b.sphere))
				{
					//do something
				}
			}

			for (size_t jdx = idx + 1; jdx < colliders.boxes.size(); ++jdx)
			{
				auto& a = colliders.boxes[jdx];
				if (math::intersects(b.sphere, a.box))
				{
					//do something
				}
			}
		}
		//resolve collisions
	}
}