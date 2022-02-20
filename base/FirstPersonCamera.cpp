#include "FirstPersonCamera.h"

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::SetPosition(float x, float y, float z)
{
	SetPosition(glm::vec3(x, y, z));
}

void FirstPersonCamera::SetPosition(const glm::vec3& pos)
{
	m_Transform.SetPosition(pos);
}

void FirstPersonCamera::LookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
{
	m_Transform.SetPosition(pos);
	m_Transform.LookAt(target, up);
}

void FirstPersonCamera::LookTo(const glm::vec3& pos, const glm::vec3& to, const glm::vec3& up)
{
	m_Transform.SetPosition(pos);
	m_Transform.LookTo(to, up);
}

void FirstPersonCamera::Strafe(float d)
{
	m_Transform.Translate(m_Transform.GetRightAxis(), d);
}

void FirstPersonCamera::Walk(float d)
{
	glm::vec3 rightVec = m_Transform.GetRightAxis();
	glm::vec3  frontVec = glm::normalize(glm::cross(rightVec, glm::vec3(0.,1.,0.)));//??

	m_Transform.Translate(frontVec, d);
}

void FirstPersonCamera::MoveForward(float d)
{
	m_Transform.Translate(m_Transform.GetForwardAxis(), d);
}

void FirstPersonCamera::Pitch(float rad)
{
	glm::vec3 rotation = m_Transform.GetRotation();
	// 将绕x轴旋转弧度限制在[-7pi/18, 7pi/18]之间
	rotation.x += rad;
	if (rotation.x > glm::pi<float>() * 7 / 18)
		rotation.x = glm::pi<float>() * 7 / 18;
	else if (rotation.x < -glm::pi<float>() * 7 / 18)
		rotation.x = -glm::pi<float>() * 7 / 18;

	m_Transform.SetRotation(rotation);
}

void FirstPersonCamera::RotateY(float rad)
{
	glm::vec3 rotation = m_Transform.GetRotation();
	//	rotation.y = XMScalarModAngle(rotation.y + rad);
	rotation.y =rotation.y + rad;
	m_Transform.SetRotation(rotation);
}



//


//////glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
//auto R = glm::yawPitchRoll(0.7853981633974483, 0., 0.);



//////auto R = glm::translate(glm::mat4(1.0f), glm::vec3(1.,2.,3.));


//glm::vec4 some = { 1,0,0,1 };

//some = R * some ;


////////for (int i = 0; i < 4; i++) {

////////	for (int j = 0; j < 4; j++) {
////////		std::cout << R[i][j] << "            ";

////////	}
////////	std::cout << std::endl;
////////}


//for (int i = 0; i < 4; i++) {
//	std::cout << some[i] << "    ";
//}

//std::cout << std::endl;


