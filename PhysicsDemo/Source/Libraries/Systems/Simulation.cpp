#include "Simulation.h"


#include "Components.h"

namespace jm
{
	constexpr f32 Damping = 0.9995f;
	constexpr math::vector3<f32> Gravity = { 0.f, -9.81f, 0.f };
	constexpr math::vector2<f32> Gravity2 = { Gravity.x, Gravity.y };

	void integrate(entity_registry& registry, f32 delta_time, math::vector3<f32> wind_force)
	{
		{
			auto lin_sim_view = registry.view<spatial2_component, linear_body2_component>();
			for (auto&& [entity, spatial, linear] : lin_sim_view.each())
			{
				math::vector2_f32 acceleration = Gravity2 + linear.applied_force * linear.inverse_mass;
				math::euler_integration(linear.velocity, acceleration, delta_time);
				linear.velocity *= Damping;
				math::euler_integration(spatial.position, linear.velocity, delta_time);
			}
		}
		{
			auto lin_sim_view = registry.view<spatial3_component, linear_body3_component, pinned_component, sphere_shape_component>();
			for (auto&& [entity, spatial, linear, pinned, sphere] : lin_sim_view.each())
			{
				if (!pinned.isPinned)
				{
					math::vector3_f32 acceleration = Gravity + wind_force + linear.applied_force * linear.inverse_mass;
					math::euler_integration(linear.velocity, acceleration, delta_time);
					linear.velocity *= Damping;
					math::euler_integration(spatial.position, linear.velocity, delta_time);

					if (spatial.position.y < 0 + sphere.radius)
					{
						spatial.position = { spatial.position.x, sphere.radius, spatial.position.z };
					}
				}
			}
		}
		{
			auto ang_sim_view = registry.view<spatial2_component, rotational_body2_component>();
			for (auto&& [entity, spatial, angular] : ang_sim_view.each())
			{
				f32 acceleration = angular.inverse_inertia * angular.applied_torque;
				math::euler_integration(angular.velocity, acceleration, delta_time);
				angular.velocity *= Damping;
				math::euler_integration(spatial.orientation, angular.velocity, delta_time);
			}
		}
		{
			auto ang_sim_view = registry.view<spatial3_component, rotational_body3_component, pinned_component>();
			for (auto&& [entity, spatial, angular, pinned] : ang_sim_view.each())
			{
				if (!pinned.isPinned)
				{
					const math::matrix33_f32 rotationMatrix = math::rotation_matrix3(spatial.orientation);
					const math::matrix33_f32 inverse_inertia_world = rotationMatrix * math::diagonal_matrix3(angular.inverse_inertia) * transpose(rotationMatrix);
					const math::vector3_f32 acceleration = inverse_inertia_world * angular.applied_torque;
					math::euler_integration(angular.velocity, acceleration, delta_time);
					angular.velocity *= Damping;
					const math::quaternion_f32 spin = math::get_spin(spatial.orientation, angular.velocity);
					math::euler_integration(spatial.orientation, spin, delta_time);
					spatial.orientation = normalize(spatial.orientation);
				}
			}
		}
		//{
		//	auto constraints = registry.view<constraint_component>();
		//	for (auto&& [entity, constraint] : constraints.each())
		//	{
		//		spatial3_component& massAPos = registry.get<spatial3_component>(constraint.massA);
		//		spatial3_component& massBPos = registry.get<spatial3_component>(constraint.massB);
		//		for (int i = 0; i < 3; ++i) //relaxation, apply multiple times
		//		{
		//			const math::vector3_f32 dist = massAPos.position - massBPos.position;
		//			const math::vector3_f32 dir = normalize(dist);
		//			f32 d = length(dist);

		//			f32 difference = std::abs(constraint.linkDistance - d) * 0.95f;

		//			math::vector3_f32 translate = dir * 0.5f * difference;

		//			massAPos.position -= translate;
		//			massBPos.position += translate;
		//		}
		//	}
		//}
		{
			for (int i = 0; i < 12; ++i) //relaxation, apply multiple times
			{
				auto constraints_rigid = registry.view<constraint_component_rigid>();
				for (auto&& [entity, constraint] : constraints_rigid.each())
				{
					spatial3_component& massAPos = registry.get<spatial3_component>(constraint.massA);
					spatial3_component& massBPos = registry.get<spatial3_component>(constraint.massB);
					pinned_component& massAPin = registry.get<pinned_component>(constraint.massA);
					pinned_component& massBPin = registry.get<pinned_component>(constraint.massB);

					const math::vector3_f32 dist = massAPos.position - massBPos.position;
					f32 magnitude = length(dist);
					if (magnitude > (constraint.breakThreshold * constraint.linkDistance))
					{
						registry.destroy(entity);
					}
					const math::vector3_f32 dir = normalize(dist);

					if (!massBPin.isPinned && !massAPin.isPinned)
					{
						const math::vector3_f32 midpoint = (massAPos.position + massBPos.position) / 2.0f;
						massBPos.position = -(0.5f * dir * constraint.linkDistance) + midpoint;
						massAPos.position = 0.5f * dir * constraint.linkDistance + midpoint;
					}
					else if (massBPin.isPinned)
					{
						massAPos.position = dir * constraint.linkDistance + massBPos.position;
					}
					else if (massAPin.isPinned)
					{
						massBPos.position = -(dir * constraint.linkDistance) + massAPos.position;
					}
				}
			}
		}
	}
}