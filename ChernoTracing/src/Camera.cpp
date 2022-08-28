#include "Camera.h"

#include "Walnut/Input/Input.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Walnut;

/*****************************************************************************/
// c_Camera

/*===========================================================================*/
c_Camera::c_Camera
( const float VerticalFOV
, const float NearClip
, const float FarClip
)
	: m_VerticalFOV(VerticalFOV)
	, m_NearClip(NearClip)
	, m_FarClip(FarClip)
	, m_Position(0, 0, 3)
	, m_ForwardDirection(0, 0, -1)
{
}

/*===========================================================================*/
void c_Camera::OnUpdate(const float TimeDelta)
{
	const glm::vec2 MousePos = Input::GetMousePosition();
	const glm::vec2 MouseDelta = MousePos - m_LastMousePosition;
	m_LastMousePosition = MousePos;

	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	Input::SetCursorMode(CursorMode::Locked);

	bool Moved = false;

	// TODO: Need to calculate up direction based on forward direction.
	constexpr glm::vec3 UpDirection = m_UpAxis;

	const glm::vec3 RightDirection = glm::cross(m_ForwardDirection, m_UpAxis);

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
	if (MouseDelta != glm::vec2{0.0f, 0.0f})
	{
		const float PitchDelta = MouseDelta.y * m_RotationSpeed;
		const float YawDelta = MouseDelta.x * m_RotationSpeed;

		const glm::quat q
			= glm::normalize
				( glm::cross
					( glm::angleAxis(-PitchDelta, RightDirection)
					, glm::angleAxis(-YawDelta, m_UpAxis)
					)
				);

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
void c_Camera::OnResize(const uint32_t Width, const uint32_t Height)
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
	m_Projection
		= glm::perspectiveFov
			( glm::radians(m_VerticalFOV)
			, static_cast<float>(m_ViewportWidth)
			, static_cast<float>(m_ViewportHeight)
			, m_NearClip
			, m_FarClip
			);

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

	for (uint32_t y = 0; y < m_ViewportHeight; ++y)
	{
		for (uint32_t x = 0; x < m_ViewportWidth; ++x)
		{
			glm::vec2 PixelUV
				( static_cast<float>(x) / m_ViewportWidth
				, static_cast<float>(y) / m_ViewportHeight
				);
			PixelUV = PixelUV * 2.0f - 1.0f; // -1 -> 1

			const glm::vec4 Target
				= m_InverseProjection * glm::vec4(PixelUV.x, PixelUV.y, 1, 1);

			// World space
			const glm::vec3 RayDirection
				= m_InverseView
					* glm::vec4(glm::normalize(glm::vec3(Target) / Target.w), 0);

			m_RayDirections[x + y * m_ViewportWidth] = RayDirection;
		}
	}
}

/*===========================================================================*/
