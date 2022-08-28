#pragma once

#include <glm/glm.hpp>
#include <vector>

/*****************************************************************************/
// c_Camera

class c_Camera
{
	//--- Public Methods ------------------------------------------------------
	public:
		c_Camera(float VerticalFOV, float NearClip, float FarClip);

		void OnUpdate(float TimeDelta);
		void OnResize(uint32_t Width, uint32_t Height);

		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }
		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetInverseView() const { return m_InverseView; }
	
		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetDirection() const { return m_ForwardDirection; }

		const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }

	//--- Private Static Members ----------------------------------------------
	private:
		static constexpr glm::vec3 m_UpAxis = glm::vec3(0.f, 1.0f, 0.0f);

		static constexpr float m_MovementSpeed = 5.0f;
		static constexpr float m_RotationSpeed = 0.3f;

	//--- Private Methods -----------------------------------------------------
	private:
		void RecalculateProjection();
		void RecalculateView();
		void RecalculateRayDirections();

	//--- Private Members -----------------------------------------------------
	private:
		glm::mat4 m_Projection{ 1.0f };
		glm::mat4 m_View{ 1.0f };
		glm::mat4 m_InverseProjection{ 1.0f };
		glm::mat4 m_InverseView{ 1.0f };

		float m_VerticalFOV = 45.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 100.0f;

		glm::vec3 m_Position{0.0f, 0.0f, 0.0f};
		glm::vec3 m_ForwardDirection{0.0f, 0.0f, 0.0f};

		// Cached ray directions
		std::vector<glm::vec3> m_RayDirections;

		glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

/*****************************************************************************/
