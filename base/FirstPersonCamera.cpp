#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera(glm::vec3 position_) :
    position(position_),
    //单位四元数
    orientation{glm::quat(1, 0, 0, 0)},
    yaw_angle{0.f},
    pitch_angle{0.f},
    roll_angle{0.f},
    movement_speed(speed_),
    mouse_sensitivity(sensitivity_)
{
	UpdateCameraVectors();
}

FirstPersonCamera::FirstPersonCamera(float posX, float posY, float posZ) :
    position(glm::vec3(posX, posY, posZ)),
    //单位四元数
    orientation{glm::quat(1, 0, 0, 0)},
    yaw_angle{0.f},
    pitch_angle{0.f},
    roll_angle{0.f},
    movement_speed(speed_),
    mouse_sensitivity(sensitivity_)
{
	UpdateCameraVectors();
}

//void FirstPersonCamera::LookAt(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up)
//{
//	m_Transform.SetPosition(pos);
//	m_Transform.LookAt(target, up);
//}
//
//void FirstPersonCamera::LookTo(const glm::vec3 &pos, const glm::vec3 &to, const glm::vec3 &up)
//{
//	m_Transform.SetPosition(pos);
//	m_Transform.LookTo(to, up);
//}

//void FirstPersonCamera::Strafe(float d)
//{
//	m_Transform.Translate(m_Transform.GetRightAxis(), d);
//}
//
//void FirstPersonCamera::Walk(float d)        //不论朝向地径直前进
//{
//	const glm::vec3 rightVec = m_Transform.GetRightAxis();
//	const glm::vec3 frontVec = glm::normalize(glm::cross(rightVec, glm::vec3(0., 1., 0.)));
//
//	m_Transform.Translate(-frontVec, d);
//}
//
//void FirstPersonCamera::MoveForward(float d)
//{
//	m_Transform.Translate(m_Transform.GetForwardAxis(), d);
//}
//
glm::mat4 FirstPersonCamera::GetViewMatrix() const
{
	// You should know the camera move reversely relative to the user input.
	// That's the point of Graphics Camera

	//范数为1的四元数的逆就是这个四元数的共轭
	const glm::quat reverse_orient = glm::conjugate(orientation);
	const glm::mat4 rot            = glm::mat4_cast(reverse_orient);
	const glm::mat4 translation    = glm::translate(glm::mat4(1.0), -position);
	return rot * translation;
}

void FirstPersonCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	const float velocity = movement_speed * deltaTime;

	const auto qff = orientation * glm::vec3(0, 0, -1);
	//const glm::vec3 front_vec = glm::rotate(orientation, glm::vec3(0.0, 0.0, -1.0));

	//中间这个四元数被称为纯四元数
	const glm::quat qF = orientation * glm::quat(0, 0, 0, -1) * glm::conjugate(orientation);
	const glm::quat qU = orientation * glm::quat(0, 0, 1, 0) * glm::conjugate(orientation);

	const glm::vec3 Front = {qF.x, qF.y, qF.z};
	const glm::vec3 Up    = {qU.x, qU.y, qU.z};

	const glm::vec3 Right = glm::normalize(glm::cross(Front, Up));

	if (direction == Camera_Movement::Forward)
	{
		position += Front * velocity;
	}

	if (direction == Camera_Movement::Backward)
	{
		position -= Front * velocity;
	}

	if (direction == Camera_Movement::Left)
	{
		position -= Right * velocity;
	}

	if (direction == Camera_Movement::Right)
	{
		position += Right * velocity;
	}

	if (direction == Camera_Movement::RollLeft)
	{
		roll_angle -= velocity;
		UpdateCameraVectors();
	}

	if (direction == Camera_Movement::RollRight)
	{
		roll_angle += velocity;
		UpdateCameraVectors();
	}

	//if (direction == Camera_Movement::ZoomIn)
	//{
	//	ProcessMouseScroll(zoom_rate);
	//}

	//if (direction == Camera_Movement::ZoomOut)
	//{
	//	ProcessMouseScroll(-zoom_rate);
	//}
}

void FirstPersonCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mouse_sensitivity;
	yoffset *= mouse_sensitivity;

	yaw_angle -= xoffset;
	pitch_angle -= yoffset;

	UpdateCameraVectors();
}

void FirstPersonCamera::ProcessMouseScroll(float scroll_offset)
{
	if (m_FovY >= min_fov && m_FovY <= max_fov)
	{
		m_FovY -= scroll_offset;
	}

	if (m_FovY <= min_fov)
	{
		m_FovY = min_fov;
	}
	if (m_FovY >= max_fov)
	{
		m_FovY = max_fov;
	}
}

glm::vec3 FirstPersonCamera::GetEyePos() const
{
	return position;
}

void FirstPersonCamera::UpdateCameraVectors()
{
	// Yaw
	const glm::quat aroundY = glm::angleAxis(glm::radians(yaw_angle), glm::vec3(0, 1, 0));
	// Pitch
	const glm::quat aroundX = glm::angleAxis(glm::radians(pitch_angle), glm::vec3(1, 0, 0));
	// Roll
	const glm::quat aroundZ = glm::angleAxis(glm::radians(roll_angle), glm::vec3(0, 0, 1));

	//yxz
	orientation = aroundY * aroundX * aroundZ;
}
