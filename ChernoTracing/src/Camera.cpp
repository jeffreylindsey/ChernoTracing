#include "Camera.h"

#include "Walnut/Input/Input.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Walnut;

/*****************************************************************************/
// c_Camera

/*===========================================================================*/
c_Camera::c_Camera(float VerticalFOV, float NearClip, float FarClip)
	: m_VerticalFOV(VerticalFOV), m_NearClip(NearClip), m_FarClip(FarClip)
{
	m_ForwardDirection = glm::vec3(0, 0, -1);
	m_Position = glm::vec3(0, 0, 3);
}

/*===========================================================================*/
void c_Camera::OnUpdate(float TimeDelta)
{
	glm::vec2 MousePos = Input::GetMousePosition();
	glm::vec2 Delta = (MousePos - m_LastMousePosition) * 0.002f;  // TODO: Combine this scaler into m_RotationSpeed.
	m_LastMousePosition = MousePos;

	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	Input::SetCursorMode(CursorMode::Locked);

	bool Moved = false;

	constexpr glm::vec3 UpDirection = m_UpAxis;  // TODO: Need to calculate up direction based on forward direction.
	glm::vec3 RightDirection = glm::cross(m_ForwardDirection, m_UpAxis);

	// Movement
	if (Input::IsKeyDown(KeyCode::W))
	{
		m_Position += m_ForwardDirection * m_MovementSpeed * TimeDelta;
		Moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::S))
	{
		m_Position -= m_ForwardDirection * m_MovementSpeed * TimeDelta;
		Moved = true;
	}
	if (Input::IsKeyDown(KeyCode::A))
	{
		m_Position -= RightDirection * m_MovementSpeed * TimeDelta;
		Moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::D))
	{
		m_Position += RightDirection * m_MovementSpeed * TimeDelta;
		Moved = true;
	}
	if (Input::IsKeyDown(KeyCode::Q))
	{
		m_Position -= UpDirection * m_MovementSpeed * TimeDelta;
		Moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::E))
	{
		m_Position += UpDirection * m_MovementSpeed * TimeDelta;
		Moved = true;
	}

	// Rotation
	if (Delta.x != 0.0f || Delta.y != 0.0f)
	{
		float PitchDelta = Delta.y * m_RotationSpeed;
		float YawDelta = Delta.x * m_RotationSpeed;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-PitchDelta, RightDirection),
			glm::angleAxis(-YawDelta, m_UpAxis)));
		m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

		Moved = true;
	}

	if (Moved)
	{
		RecalculateView();
		RecalculateRayDirections();
	}
}

/*===========================================================================*/
void c_Camera::OnResize(uint32_t Width, uint32_t Height)
{
	if (Width == m_ViewportWidth && Height == m_ViewportHeight)
		return;

	m_ViewportWidth = Width;
	m_ViewportHeight = Height;

	RecalculateProjection();
	RecalculateRayDirections();
}

/*===========================================================================*/
void c_Camera::RecalculateProjection()
{
	m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	m_InverseProjection = glm::inverse(m_Projection);
}

/*===========================================================================*/
void c_Camera::RecalculateView()
{
	m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, m_UpAxis);
	m_InverseView = glm::inverse(m_View);
}

/*===========================================================================*/
void c_Camera::RecalculateRayDirections()
{
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	for (uint32_t y = 0; y < m_ViewportHeight; y++)
	{
		for (uint32_t x = 0; x < m_ViewportWidth; x++)
		{
			glm::vec2 PixelUV = { (float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight };
			PixelUV = PixelUV * 2.0f - 1.0f; // -1 -> 1

			glm::vec4 Target = m_InverseProjection * glm::vec4(PixelUV.x, PixelUV.y, 1, 1);
			glm::vec3 RayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(Target) / Target.w), 0)); // World space
			m_RayDirections[x + y * m_ViewportWidth] = RayDirection;
		}
	}
}

/*===========================================================================*/
