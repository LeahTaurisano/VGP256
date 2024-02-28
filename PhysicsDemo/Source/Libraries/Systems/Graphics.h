#pragma once

#include "Visual/RenderingContext.h"
#include "Visual/Visual.h"

#include "Math/Camera.h"

#include "Entity.h"

namespace jm
{
	namespace Platform
	{
		class MessageHandler;
	}
}

namespace jm::System
{
	class Graphics
	{
		struct Data2D
		{
			Data2D(cstring vertexSource, cstring fragmentSource);

			Visual::ShaderProgram Program;
			OpenGL::InputLayoutHandle inputLayoutHandle;
			OpenGL::InputBufferHandle inputBufferHandle;
			GLsizei squareVertices;
			GLsizei diskVertices;
			GLsizei axesVertices;
		};

		struct Data3D
		{
			Data3D(cstring vertexSource, cstring fragmentSource);

			Visual::ShaderProgram Program;
			OpenGL::InputLayoutHandle inputLayoutHandle;
			OpenGL::InputBufferHandle inputBufferHandle;
			GLsizei cubeVertices;
			GLsizei sphereVertices;
			GLsizei axesVertices;
		};


		Rendering::Context Renderer;
		entity_registry& EntityRegistry;
		Data2D TwoDimensional;
		Data3D ThreeDimensional;

		math::vector3_f32 ClearColour;
		bool Debug2D = false;
		bool Debug3D = true;

	public:

		Graphics(Platform::Window& window, entity_registry& registry, math::vector3_f32 const& clearColour);

		~Graphics();

		Platform::MessageHandler* GetMessageHandler();

		void Draw(math::camera3<f32> const& camera, std::function<void()>&& imguiFrame);

		void ImGuiDebug();
	};
}