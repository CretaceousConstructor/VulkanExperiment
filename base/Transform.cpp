#include "Transform.h"

Transform::Transform(const glm::vec3 &scale, const glm::vec3 &rotation, const glm::vec3 &position)

    :
    m_Scale(scale),
    m_Rotation(rotation),
    m_Position(position)
{
}

glm::vec3 Transform::GetScale() const
{
	return m_Scale;
}

glm::vec3 Transform::GetRotation() const
{
	return m_Rotation;
}

glm::vec3 Transform::GetPosition() const
{
	return m_Position;
}

glm::vec3 Transform::GetRightAxis() const
{
	//x == pitch
	//y = yaw
	//z = roll
	auto R = glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//GLM 都是列主矩阵
	glm::vec3 result = R[0];
	return result;
}

glm::vec3 Transform::GetUpAxis() const
{
	//x == pitch
	//y = yaw
	//z = roll
	auto R = glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//GLM 都是列主矩阵

	glm::vec3 result = R[1];

	return result;
}

glm::vec3 Transform::GetForwardAxis() const
{
	//x == pitch
	//y = yaw
	//z = roll
	auto R = glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//GLM 都是列主矩阵

	glm::vec3 result = R[2];

	return result;
}

glm::mat4x4 Transform::GetLocalToWorldMatrix() const
{
	auto world = glm::translate(glm::mat4(1.0f), m_Position) * glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z) *
	             glm::scale(glm::mat4(1.0f), m_Scale);
	return world;
}

glm::mat4x4 Transform::GetWorldToLocalMatrix() const
{
	auto InvWorld = glm::inverse(GetLocalToWorldMatrix());

	return InvWorld;
}

void Transform::SetScale(const glm::vec3 &scale)
{
	m_Scale = scale;
}

void Transform::SetScale(float x, float y, float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

void Transform::SetRotation(const glm::vec3 &eulerAnglesInRadian)
{
	m_Rotation = eulerAnglesInRadian;
}

void Transform::SetRotation(float x, float y, float z)
{
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
}

void Transform::SetPosition(const glm::vec3 &position)
{
	m_Position = position;
}

void Transform::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void Transform::RotateAxis(const glm::vec3 &axis, float radian)
{
	auto R = glm::rotate(glm::mat4(1.0f), radian, glm::vec3(0.0, 0.0, 1.0)) * glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);

	glm::extractEulerAngleYXZ(R, m_Rotation.y, m_Rotation.x, m_Rotation.z);
}

void Transform::Rotate(const glm::vec3 &eulerAnglesInRadian)
{
	m_Rotation = m_Rotation + eulerAnglesInRadian;
}

void Transform::Translate(const glm::vec3 &direction, float magnitude)
{
	m_Position = m_Position + magnitude * direction;
}

void Transform::LookAt(const glm::vec3 &target, const glm::vec3 &up)
{
	auto view    = glm::lookAtRH(m_Position, target, up);
	auto InvView = glm::inverse(view);

	glm::extractEulerAngleYXZ(InvView, m_Rotation.y, m_Rotation.x, m_Rotation.z);
}

void Transform::LookTo(const glm::vec3 &direction, const glm::vec3 &up)
{
	auto target = m_Position + direction;
	this->LookAt(target, up);
}
