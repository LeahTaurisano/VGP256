#include "Visual/RenderingContext.h"
#include "Visual/DearImGui/ImGuiContext.h"

#include "Math/Camera.h"

#include "Platform/Tactual.h"
#include "Platform/WindowedApplication.h"

#include "Systems/Entity.h"
#include "Systems/Collision.h"
#include "Systems/Graphics.h"

#include "DearImGui/imgui.h"

#include "Systems/Components.h"

#include "Worlds.h"

namespace jm
{
	constexpr math::vector2<iSize> screenSize = { 1600, 900 };
	constexpr math::vector2_f32 screenSizeFloat = { f32(screenSize.x), f32(screenSize.y) };

	math::camera3<f32> Make3DCamera(f32 distanceFromOrigin, f32 yFOV, f32 aspectRatio)
	{
		math::vector3_f32 lookFrom = distanceFromOrigin * math::vector3_f32{ 1.0f };
		math::vector3_f32 lookAt = { 0.0f, 0.0f, 0.0f };
		return math::camera3<f32>(lookFrom, lookAt, yFOV, aspectRatio);
	}

	math::vector2_f32 GetWorldFromScreenPosition(math::camera3<f32> const& camera, Platform::ScreenPosition screen)
	{
		math::vector2_f32 screenPositionFloat = { screen.X, screen.Y };
		math::vector2_f32 screenNormed = 2.0f * ((screenPositionFloat / screenSizeFloat) - 0.5f);
		screenNormed.y = -screenNormed.y;
		return camera.get_screen_to_world(screenNormed);
	}

	struct PhysicsDemo : Platform::WindowedApplication
	{
		PhysicsDemo(const Platform::RuntimeContext& context)
			: Platform::WindowedApplication(context, { "3D", { 50 , 50 }, { screenSize.x, screenSize.y } })
			, Camera(Make3DCamera(10.0f, 45.0f, window->GetArea().GetAspectRatio()))
			, ClearColour({ 0.2f, 0.3f, 0.3f })
			, registry()
			, InputSystem()
			, GraphicsSystem(*window, registry)
		{
			Timer.Initialize();
		}

		virtual ~PhysicsDemo() override = default;

		virtual void OnStartLoop() override
		{
			AddMessageHandler(GraphicsSystem.GetMessageHandler());
			AddMessageHandler(InputSystem.GetMessageHandler());
			CreateWorld();
		}

		virtual void RunLoop() override
		{
			Timer.Update();
			InputUpdate();

			Colliders = build_colliders(registry);

			GraphicsSystem.Draw3D(Camera, ClearColour, []() {});

			Running = !InputSystem.GetKeyboard().EscPressed;
		}

		virtual void OnStopLoop() override
		{
			DestroyWorld();
			RemoveMessageHandler(InputSystem.GetMessageHandler());
			RemoveMessageHandler(GraphicsSystem.GetMessageHandler());
		}

		void HandleException(const std::exception& applicationException)
		{
			JM_HALT("Application", applicationException.what());
		}

		void CreateWorld()
		{
			CreateBasicWorld(registry);
		}

		void DestroyWorld()
		{
			registry.clear();
		}

		void InputUpdate()
		{
			const bool shiftPressed = InputSystem.GetKeyboard().ShiftPressed;
			const float cameraTranslateSpeed = (shiftPressed ? 3.0f : 1.5f) * float(Timer.GetElapsedTime());
			const float cameraRotateSpeed = float(Timer.GetElapsedTime());

			if (InputSystem.GetKeyboard().WPressed)
			{
				Camera.translate(cameraTranslateSpeed * Camera.get_forward());
			}
			if (InputSystem.GetKeyboard().SPressed)
			{
				Camera.translate(cameraTranslateSpeed * Camera.get_back());
			}
			if (InputSystem.GetKeyboard().APressed)
			{
				Camera.translate(cameraTranslateSpeed * Camera.get_left());
			}
			if (InputSystem.GetKeyboard().DPressed)
			{
				Camera.translate(cameraTranslateSpeed * Camera.get_right());
			}
			if (InputSystem.GetKeyboard().UpPressed)
			{
				Camera.rotate(0.0f, cameraRotateSpeed);
			}
			if (InputSystem.GetKeyboard().DownPressed)
			{
				Camera.rotate(0.0f, -cameraRotateSpeed);
			}
			if (InputSystem.GetKeyboard().LeftPressed)
			{
				Camera.rotate(-cameraRotateSpeed, 0.0f);
			}
			if (InputSystem.GetKeyboard().RightPressed)
			{
				Camera.rotate(cameraRotateSpeed, 0.0f);
			}
			InputSystem.Update();
		}

		Platform::Timer Timer;

		entity_registry registry;

		math::camera3<f32> Camera;
		math::vector3_f32 ClearColour;
		collider_set Colliders;

		Tactual::System InputSystem;
		System::Graphics GraphicsSystem;
	};
}


JM_APPLICATION_MAIN("Physics Demo", jm::PhysicsDemo)
