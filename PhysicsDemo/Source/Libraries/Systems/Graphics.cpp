#include "Graphics.h"
#include "Components.h"

#include "Visual/DearImGui/ImGuiContext.h"
#include "Visual/VisualGeometry.h"

#include "Platform/WindowedApplication.h"

namespace jm::System
{
	Graphics::Data2D::Data2D(cstring vertexSource, cstring fragmentSource)
		: Program(vertexSource, fragmentSource)
	{
	}

	Graphics::Data3D::Data3D(cstring vertexSource, cstring fragmentSource)
		: Program(vertexSource, fragmentSource)
	{
	}

	cstring pixelShader = R"(
			#version 330 core
			
			out vec4 FragColor;
			
			in vec3 outColour;
			
			void main()
			{
			    FragColor = vec4(outColour, 1.0f);
			}
			)";

	Graphics::Graphics(Platform::Window& window, entity_registry& registry, math::vector3_f32 const& clearColour)
		: Renderer(window)
		, EntityRegistry(registry)
		, TwoDimensional(R"(
			#version 330 core
			layout (location = 0) in vec2 inPosition;
			layout (location = 1) in vec3 inColour;
			  
			out vec3 outColour;
			
			uniform mat3 view;
			uniform mat3 model;
			
			void main()
			{
				vec3 worldPosition = view * model * vec3(inPosition, -1.0);
			    gl_Position = vec4(worldPosition, 1.0);
				outColour = inColour;
			}
			)", pixelShader)
		, ThreeDimensional(R"(
			#version 330 core
			layout (location = 0) in vec3 inPosition;
			layout (location = 1) in vec3 inColour;
			  
			out vec3 outColour;
			
			uniform mat4 projectionView;
			uniform mat4 model;
			
			void main()
			{
			    gl_Position = projectionView * model * vec4(inPosition, 1.0);
				outColour = inColour;
			}
			)", pixelShader)
		, ClearColour(clearColour)
	{
		{
			Visual::InputLayout layout{ {2, 3 } };

			byte_list inputVertexData;
			{
				auto boxVertexData = Visual::GenerateBox(layout);
				inputVertexData.insert(inputVertexData.end(), boxVertexData.data.begin(), boxVertexData.data.end());
				TwoDimensional.squareVertices = (GLsizei)boxVertexData.size;
			}
			{
				auto diskVertexData = Visual::GenerateDisk(layout);
				inputVertexData.insert(inputVertexData.end(), diskVertexData.data.begin(), diskVertexData.data.end());
				TwoDimensional.diskVertices = (GLsizei)diskVertexData.size;
			}
			{
				auto axesVertexData = Visual::GenerateCoordinateAxes2(layout);
				inputVertexData.insert(inputVertexData.end(), axesVertexData.data.begin(), axesVertexData.data.end());
				TwoDimensional.axesVertices = (GLsizei)axesVertexData.size;
			}

			TwoDimensional.Program.MakeActive();
			TwoDimensional.inputLayoutHandle = Renderer.RasterizerMemory->createInputLayout(layout);
			TwoDimensional.inputBufferHandle = Renderer.RasterizerMemory->createInputBuffer(TwoDimensional.inputLayoutHandle, inputVertexData);

			ThreeDimensional.linesLayoutHandle = Renderer.RasterizerMemory->createInputLayout(layout);
		}
		{
			Visual::InputLayout layout{ { 3, 3 } };

			byte_list inputVertexData;
			{
				auto cubeVertexData = Visual::GenerateCube(layout);
				inputVertexData.insert(inputVertexData.end(), cubeVertexData.data.begin(), cubeVertexData.data.end());
				ThreeDimensional.cubeVertices = (GLsizei)cubeVertexData.size;
			}
			{
				auto sphereVertexData = Visual::GenerateSphere(layout);
				inputVertexData.insert(inputVertexData.end(), sphereVertexData.data.begin(), sphereVertexData.data.end());
				ThreeDimensional.sphereVertices = (GLsizei)sphereVertexData.size;
			}
			{
				auto axesVertexData = Visual::GenerateCoordinateAxes3(layout);
				inputVertexData.insert(inputVertexData.end(), axesVertexData.data.begin(), axesVertexData.data.end());
				ThreeDimensional.axesVertices = (GLsizei)axesVertexData.size;
			}

			ThreeDimensional.Program.MakeActive();
			ThreeDimensional.inputLayoutHandle = Renderer.RasterizerMemory->createInputLayout(layout);
			ThreeDimensional.inputBufferHandle = Renderer.RasterizerMemory->createInputBuffer(ThreeDimensional.inputLayoutHandle, inputVertexData);
		}

		glEnable(GL_DEPTH_TEST);
	}

	Graphics::~Graphics()
	{
		Renderer.RasterizerMemory->destroyInputBuffer(TwoDimensional.inputLayoutHandle, TwoDimensional.inputBufferHandle);
		Renderer.RasterizerMemory->destroyInputLayout(TwoDimensional.inputLayoutHandle);

		Renderer.RasterizerMemory->destroyInputBuffer(ThreeDimensional.inputLayoutHandle, ThreeDimensional.inputBufferHandle);
		Renderer.RasterizerMemory->destroyInputLayout(ThreeDimensional.inputLayoutHandle);
	}

	Platform::MessageHandler* Graphics::GetMessageHandler()
	{
		return Renderer.ImGuiContextPtr->GetMessageHandler();
	}

	template <typename T>
	auto&& get_shapes2(entity_registry& registry)
	{
		auto shape_entity_view = registry.view<const T, const spatial2_component>();
		return shape_entity_view.each();
	}

	template <typename T>
	auto&& get_shapes3(entity_registry& registry)
	{
		auto shape_entity_view = registry.view<const T, const spatial3_component>();
		return shape_entity_view.each();
	}

	void Graphics::Draw(math::camera3<f32> const& camera, std::function<void()>&& imguiFrame)
	{
		std::vector<math::matrix33_f32> diskInstances;
		std::vector<math::matrix33_f32> squareInstances;
		std::vector<math::matrix44_f32> sphereInstances;
		std::vector<math::matrix44_f32> cubeInstances;
		{
			auto disk_entity_view = EntityRegistry.view<const disk_shape_component, const spatial2_component>();
			diskInstances.reserve(disk_entity_view.size_hint());
			for (auto&& [entity, shape, spatial] : disk_entity_view.each())
			{
				diskInstances.push_back(math::isometry_matrix2(spatial.position, spatial.orientation) * math::scale_matrix2(shape.radius));
			}
		}
		{
			auto rectangle_entity_view = EntityRegistry.view<const rectangle_shape_component, const spatial2_component>();
			squareInstances.reserve(rectangle_entity_view.size_hint());
			for (auto&& [entity, shape, spatial] : rectangle_entity_view.each())
			{
				squareInstances.push_back(math::isometry_matrix2(spatial.position, spatial.orientation) * math::scale_matrix2(shape.extents));
			}
		}
		{
			auto sphere_entity_view = EntityRegistry.view<const sphere_shape_component, const spatial3_component>();
			sphereInstances.reserve(sphere_entity_view.size_hint());
			for (auto&& [entity, shape, spatial] : sphere_entity_view.each())
			{
				sphereInstances.push_back(math::isometry_matrix3(spatial.position, spatial.orientation) * math::scale_matrix3(shape.radius));
			}
		}
		{
			auto box_entity_view = EntityRegistry.view<const box_shape_component, const spatial3_component>();
			cubeInstances.reserve(box_entity_view.size_hint());
			for (auto&& [entity, shape, spatial] : box_entity_view.each())
			{
				cubeInstances.push_back(math::isometry_matrix3(spatial.position, spatial.orientation) * math::scale_matrix3(shape.extents));
			}
		}
		std::vector<math::vector3_f32> lines;
		//{
		//	auto constraint_lines_view = EntityRegistry.view<const constraint_component_rigid>();
		//	for (auto&& [entity, constraint] : constraint_lines_view.each())
		//	{
		//		spatial3_component massAPos = EntityRegistry.get<spatial3_component>(constraint.massA);
		//		spatial3_component massBPos = EntityRegistry.get<spatial3_component>(constraint.massB);
		//		lines.push_back(massAPos.position);
		//		lines.push_back(massBPos.position);
		//	}
		//}

		Renderer.RasterizerImpl->PrepareRenderBuffer(ClearColour);

		{
			ThreeDimensional.Program.MakeActive();

			glBindVertexArray(static_cast<GLuint>(ThreeDimensional.inputLayoutHandle));
			ThreeDimensional.Program.SetUniform("projectionView", camera.get_perspective_transform() * camera.get_view_transform());

			GLsizei start = 0;
			for (auto& instance : cubeInstances)
			{
				ThreeDimensional.Program.SetUniform("model", instance);
				glDrawArrays(GL_TRIANGLES, start, ThreeDimensional.cubeVertices);
			}
			start += ThreeDimensional.cubeVertices;

			for (auto& instance : sphereInstances)
			{
				ThreeDimensional.Program.SetUniform("model", instance);
				glDrawArrays(GL_TRIANGLES, start, ThreeDimensional.sphereVertices);
			}
			start += ThreeDimensional.sphereVertices;

			if (Debug3D)
			{
				ThreeDimensional.Program.SetUniform("model", math::identity4);
				glDrawArrays(GL_LINES, start, ThreeDimensional.axesVertices);
			}

			//draw lines in world space
			Visual::InputLayout layout{ {3, 3} };
			auto linesVertexData = Visual::GenerateLines(layout, lines);
			ThreeDimensional.linesBufferHandle = Renderer.RasterizerMemory->createInputBuffer(ThreeDimensional.linesLayoutHandle, linesVertexData.data);
			ThreeDimensional.Program.SetUniform("model", math::identity4);
			glDrawArrays(GL_LINES, 0, (GLsizei)linesVertexData.size);
		}


		{

			TwoDimensional.Program.MakeActive();
			glBindVertexArray(static_cast<GLuint>(TwoDimensional.inputLayoutHandle));
			TwoDimensional.Program.SetUniform("view", math::scale_matrix2(0.1f) * math::matrix33_f32(camera.get_orthogonal_transform()));

			GLsizei start = 0;
			for (auto& instance : squareInstances)
			{
				TwoDimensional.Program.SetUniform("model", instance);
				glDrawArrays(GL_TRIANGLES, start, TwoDimensional.squareVertices);
			}
			OpenGL::CheckError();
			start += TwoDimensional.squareVertices;
			for (auto& instance : diskInstances)
			{
				TwoDimensional.Program.SetUniform("model", instance);
				glDrawArrays(GL_TRIANGLES, start, TwoDimensional.diskVertices);
			}
			start += TwoDimensional.diskVertices;


			if (Debug2D)
			{
				TwoDimensional.Program.SetUniform("model", math::identity3);
				glDrawArrays(GL_LINES, start, TwoDimensional.axesVertices);
			}
		}

		Renderer.ImGuiContextPtr->RunFrame(std::move(imguiFrame));

		Renderer.RasterizerImpl->UpdateRenderBuffer();
	}

	void Graphics::ImGuiDebug()
	{
		ImGui::Text("Graphics");
		ImGui::ColorEdit3("BG Colour", reinterpret_cast<f32*>(&ClearColour));
		ImGui::Checkbox("Debug 2D", &Debug2D);
		ImGui::Checkbox("Debug 3D", &Debug3D);
	}
}
