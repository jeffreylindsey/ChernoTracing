#pragma once

#include <glm/glm.hpp>
#include <vector>

/*****************************************************************************/
// c_Camera

class c_Camera
{
	//--- Public Methods ------------------------------------------------------
	public:
		const glm::mat4& GetProjection() const;
		const glm::mat4& GetInverseProjection() const;
		const glm::mat4& GetView() const;
		const glm::mat4& GetInverseView() const;

		const glm::vec3& GetPosition() const;
		const glm::vec3& GetDirection() const;

		const std::vector<glm::vec3>& GetRayDirections() const;

		void SetPosition(const glm::vec3& NewPosition);

		void OnUpdate(const float TimeDelta);
		void OnResize(const uint32_t Width, const uint32_t Height);

	//--- Private Static Members ----------------------------------------------
	private:
		static constexpr glm::vec3 m_UpAxis = glm::vec3(0.f, 1.0f, 0.0f);

		static constexpr float m_VerticalFOV = 45.0f;  // TODO: Clarify as angle in degrees.
		static constexpr float m_NearClip = 0.1f;
		static constexpr float m_FarClip = 100.0f;

		static constexpr float m_MovementSpeed = 5.0f;
		static constexpr float m_RotationSpeed = 0.0006f;

	//--- Private Methods -----------------------------------------------------
	private:
		void RecalculateProjection();
		void RecalculateView();
		void RecalculateRayDirections();

	//--- Private Members -----------------------------------------------------
	private:
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_InverseProjection = glm::mat4(1.0f);

		glm::mat4 m_View = glm::mat4(1.0f);
		glm::mat4 m_InverseView = glm::mat4(1.0f);

		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_ForwardDirection = glm::vec3(0.0f, 0.0f, 1.0f);

		// Cached ray directions
		std::vector<glm::vec3> m_RayDirections;

		glm::vec2 m_LastMousePosition = {};
};

/*****************************************************************************/
