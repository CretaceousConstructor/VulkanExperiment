#include "Transform.h"

Transform::Transform(const glm::vec3 &scale, const glm::vec3 &rotation_euler, const glm::vec3 &position)
    :
    scale{scale},
    position{position},
	//这里用欧拉角初始化时，要记得使得相机正方向指向-z轴 glm::vec3(0.f,180.f,0.f)
    quat4rotation{glm::quat(rotation_euler)}
{



	//quat4rotation = glm::normalize(quat4rotation);
	// Conversion from Euler angles (in radians) to Quaternion

	//// Conversion from axis-angle
	//// In GLM the angle must be in degrees here, so convert it.
	//MyQuaternion = gtx::quat4rotation::angleAxis(degrees(RotationAngle), RotationAxis);
	//kmat4 RotationMatrix = quat4rotation::toMat4(quat4rotation);
}

glm::vec3 Transform::GetScale() const
{
	return scale;
}

glm::vec3 Transform::GetRotation() const
{
	return glm::eulerAngles(quat4rotation);
}

glm::vec3 Transform::GetPosition() const
{
	return position;
}



glm::vec3 Transform::GetRightAxis() const        //相机规定面向-z轴
{
	//y = yaw
	//x == pitch
	//z = roll
	//auto R = glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//const glm::vec3 result = R[0];


	////中间这个四元数被称为纯四元数
	//const glm::quat qF    = quat4rotation * glm::quat(0, 0, 0, 1) * glm::conjugate(quat4rotation);
	auto qF    = quat4rotation * glm::vec3( 0, 0, 1);

	//const glm::vec3 Front = {qF.x, qF.y, qF.z};
	//const glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3(0, 1, 0)));


	auto            rotation_matrix = glm::toMat4(quat4rotation);
	const glm::vec3 result          = rotation_matrix[0];

	//GLM 都是列主矩阵
	return result;



}

glm::vec3 Transform::GetUpAxis() const
{
	//x == pitch
	//y = yaw
	//z = roll
	//auto R = glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//GLM 都是列主矩阵
	//const glm::vec3 result = R[1];

	auto            rotation_matrix = glm::toMat4(quat4rotation);
	const glm::vec3 result          = rotation_matrix[1];

	return result;
}

glm::vec3 Transform::GetForwardAxis() const        //相机规定面向-z轴
{
	//x == pitch
	//y = yaw
	//z = roll
	//auto R = glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//GLM 都是列主矩阵

	//const glm::vec3 result = R[2];
	auto rotation_matrix = glm::toMat4(quat4rotation);

	const glm::vec3 result = (-rotation_matrix[2]);

	return result;
}

glm::mat4x4 Transform::GetLocalToWorldMatrix() const
{
	//const auto world = glm::translate(glm::mat4(1.0f), position) * glm::yawPitchRoll(rotation_euler.y, rotation.x, m_Rotation.z) * glm::scale(glm::mat4(1.0f), m_Scale);

	const auto world = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(quat4rotation) * glm::scale(glm::mat4(1.0f), scale);


	return world;
}

glm::mat4x4 Transform::GetWorldToLocalMatrix() const
{
	const auto InvWorld = glm::inverse(GetLocalToWorldMatrix());
	return InvWorld;
}

void Transform::SetScale(const glm::vec3 &scale_)
{
	scale = scale_;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Transform::SetRotation(const glm::vec3 &eulerAnglesInRadian)
{
	quat4rotation = glm::quat(eulerAnglesInRadian);


}

void Transform::SetRotation(float x, float y, float z)
{
	//const glm::vec3 rotation_euler{x, y, z};
	glm::vec3 rotation_euler{};
	rotation_euler.x = x;
	rotation_euler.y = y;
	rotation_euler.z = z;
	quat4rotation    = glm::quat(rotation_euler);


}

void Transform::SetPosition(const glm::vec3 &position_)
{
	position = position_;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Transform::Rotate(const glm::vec3 &eulerAnglesInRadian)
{
	quat4rotation = glm::quat(eulerAnglesInRadian) * quat4rotation;
}

void Transform::Translate(const glm::vec3 &direction, float magnitude)
{
	position = position + magnitude * direction;
}

void Transform::LookAt(const glm::vec3 &target, const glm::vec3 &up)
{
	//glm::extractEulerAngleYXZ(InvView, m_Rotation.y, m_Rotation.x, m_Rotation.z);


	const auto view    = glm::lookAtRH(position, target, up);
	const auto InvView = glm::inverse(view);
	quat4rotation      = glm::quat_cast(InvView);

	quat4rotation = glm::normalize(quat4rotation);




}

void Transform::LookTo(const glm::vec3 &direction, const glm::vec3 &up)
{
	const auto target = position + direction;
	this->LookAt(target, up);


}
