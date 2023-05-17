#include "Camera.h"

Camera::~Camera() = default;

//glm::vec3 Camera::GetPosition() const
//{
//	return m_Transform.GetPosition();
//}
//void Camera::SetPosition(float x, float y, float z)
//{
//	SetPosition(glm::vec3(x, y, z));
//}
//
//void Camera::SetPosition(const glm::vec3 &pos)
//{
//	m_Transform.SetPosition(pos);
//}
//
//float Camera::GetRotationX() const
//{
//	return m_Transform.GetRotation().x;
//}
//
//float Camera::GetRotationY() const
//{
//	return m_Transform.GetRotation().y;
//}
//
//float Camera::GetRotationZ() const
//{
//	return m_Transform.GetRotation().z;
//}
//
//glm::vec3 Camera::GetRightAxis() const
//{
//	return m_Transform.GetRightAxis();
//}
//
//glm::vec3 Camera::GetUpAxis() const
//{
//	return m_Transform.GetUpAxis();
//}
//
//glm::vec3 Camera::GetLookAxis() const
//{
//	return m_Transform.GetForwardAxis();
//}
//
//glm::mat4x4 Camera::GetView() const
//{
//	return m_Transform.GetWorldToLocalMatrix();
//}
//
//glm::mat4x4 Camera::GetInverseView() const
//{
//	return glm::inverse(m_Transform.GetWorldToLocalMatrix());
//}

glm::mat4x4 Camera::GetProjMatrix(ProjectionMtxSetting setting) const
{
	glm::mat4 result;
	if (setting == ProjectionMtxSetting::FarPlaneInfReversedZ)
	{
		const float f = 1.0f / tan(m_FovY / 2.0f);

		result = glm::mat4(
		    f / m_Aspect, 0.0f, 0.0f, 0.0f,
		    0.0f, f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, -1.0f,
		    0.0f, 0.0f, m_NearZ, 0.0f);
	}
	else if (setting == ProjectionMtxSetting::ReversedZ)
	{
		result = glm::perspectiveRH_ZO(m_FovY, m_Aspect, m_FarZ, m_NearZ);
	}
	else if (setting == ProjectionMtxSetting::NormalZ)
	{
		result = glm::perspectiveRH_ZO(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	}
	else
	{
		assert(1 == 2);
	}

	return result;
}


glm::mat4x4 Camera::GetInverseProj(ProjectionMtxSetting setting) const
{

	return glm::inverse(GetProjMatrix(setting));
}

//glm::mat4x4 Camera::GetViewProj(ProjectionMtxSetting setting) const
//{
//	return GetView() * GetProjMatrix(setting);
//}

//VkViewport Camera::GetViewPort() const
//{
//	return m_ViewPort;
//}

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

//void Camera::SetViewPort(const VkViewport &viewPort)
//{
//	m_ViewPort = viewPort;
//}
//
//void Camera::SetViewPort(float X, float Y, float width, float height, float minDepth, float maxDepth)
//{
//	//m_ViewPort.TopLeftX = topLeftX;
//	//m_ViewPort.TopLeftY = topLeftY;
//	//m_ViewPort.Width = width;
//	//m_ViewPort.Height = height;
//	//m_ViewPort.MinDepth = minDepth;
//	//m_ViewPort.MaxDepth = maxDepth;
//
//	//typedef struct VkViewport {
//	//	float    x;
//	//	float    y;
//	//	float    width;
//	//	float    height;
//	//	float    minDepth;
//	//	float    maxDepth;
//	//} VkViewport;
//	assert(1 == 2);
//	//m_ViewPort.x = X;
//	//m_ViewPort.y = Y;
//
//	//m_ViewPort.height   = height;
//	//m_ViewPort.width    = width;
//	//m_ViewPort.minDepth = minDepth;
//	//m_ViewPort.maxDepth = maxDepth;
//}
