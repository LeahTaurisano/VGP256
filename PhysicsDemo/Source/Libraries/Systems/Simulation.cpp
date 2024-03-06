#include "Simulation.h"


#include "Components.h"

namespace jm
{
	constexpr f32 Damping = 0.9995f;
	constexpr math::vector3<f32> Gravity = { 0.f, -9.81f, 0.f };
	constexpr math::vector2<f32> Gravity2 = { Gravity.x, Gravity.y };

	void integrate(entity_registry& registry, f32 delta_time)
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
			auto lin_sim_view = registry.view<spatial3_component, linear_body3_component>();
			for (auto&& [entity, spatial, linear] : lin_sim_view.each())
			{
				math::vector3_f32 acceleration = Gravity + linear.applied_force * linear.inverse_mass;
				math::euler_integration(linear.velocity, acceleration, delta_time);
				linear.velocity *= Damping;
				math::euler_integration(spatial.position, linear.velocity, delta_time);
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
			auto ang_sim_view = registry.view<spatial3_component, rotational_body3_component>();
			for (auto&& [entity, spatial, angular] : ang_sim_view.each())
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
}