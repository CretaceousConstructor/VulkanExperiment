#include "Camera.h"

Camera::~Camera() = default;

glm::vec3 Camera::GetPosition() const
{
	return m_Transform.GetPosition();
}
void Camera::SetPosition(float x, float y, float z)
{
	SetPosition(glm::vec3(x, y, z));
}

void Camera::SetPosition(const glm::vec3 &pos)
{
	m_Transform.SetPosition(pos);
}

float Camera::GetRotationX() const
{
	return m_Transform.GetRotation().x;
}

float Camera::GetRotationY() const
{
	return m_Transform.GetRotation().y;
}

float Camera::GetRotationZ() const
{
	return m_Transform.GetRotation().z;
}

glm::vec3 Camera::GetRightAxis() const
{
	return m_Transform.GetRightAxis();
}

glm::vec3 Camera::GetUpAxis() const
{
	return m_Transform.GetUpAxis();
}

glm::vec3 Camera::GetLookAxis() const
{
	return m_Transform.GetForwardAxis();
}

glm::mat4x4 Camera::GetView() const
{
	return m_Transform.GetWorldToLocalMatrix();
}

glm::mat4x4 Camera::GetInverseView() const
{
	return glm::inverse(m_Transform.GetWorldToLocalMatrix());
}

glm::mat4x4 Camera::GetProj() const
{
	return glm::perspectiveRH_ZO(m_FovY, m_Aspect, m_NearZ, m_FarZ);
}

glm::mat4x4 Camera::GetInverseProj() const
{
	return glm::inverse(GetProj());
}

glm::mat4x4 Camera::GetViewProj() const
{
	return GetView() * GetProj();
}

VkViewport Camera::GetViewPort() const
{
	return m_ViewPort;
}

void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	m_FovY   = fovY;
	m_Aspect = aspect;
	m_NearZ  = nearZ;
	m_FarZ   = farZ;
}

float Camera::GetNearZ() const
{
	return m_NearZ;
}

float Camera::GetFarZ() const
{
	return m_FarZ;
}

void Camera::SetViewPort(const VkViewport &viewPort)
{
	m_ViewPort = viewPort;
}

void Camera::SetViewPort(float X, float Y, float width, float height, float minDepth, float maxDepth)
{
	//m_ViewPort.TopLeftX = topLeftX;
	//m_ViewPort.TopLeftY = topLeftY;
	//m_ViewPort.Width = width;
	//m_ViewPort.Height = height;
	//m_ViewPort.MinDepth = minDepth;
	//m_ViewPort.MaxDepth = maxDepth;

	//typedef struct VkViewport {
	//	float    x;
	//	float    y;
	//	float    width;
	//	float    height;
	//	float    minDepth;
	//	float    maxDepth;
	//} VkViewport;
	m_ViewPort.x = X;
	m_ViewPort.y = Y;

	m_ViewPort.height   = height;
	m_ViewPort.width    = width;
	m_ViewPort.minDepth = minDepth;
	m_ViewPort.maxDepth = maxDepth;
}
