#pragma once
#include "Camera.h"

class FirstPersonCamera : public Camera
{
  public:
	FirstPersonCamera()  = default;
	~FirstPersonCamera() override = default ;

	// ���������λ��Camera position
	//void SetPosition(float x, float y, float z);
	//void SetPosition(const glm::vec3 &pos);
	// ����������ĳ���Camera direction
	void LookAt(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up);
	void LookTo(const glm::vec3 &pos, const glm::vec3 &to, const glm::vec3 &up);

	// ƽ��
	void Strafe(float d);
	// ֱ��(ƽ���ƶ�)
	void Walk(float d);
	// ǰ��(��ǰ���ƶ�)
	void MoveForward(float d);

	// ���¹۲�
	// ��radֵ���Ϲ۲�
	// ��radֵ���¹۲�
	void Pitch(float rad);
	// ���ҹ۲�
	// ��radֵ����۲�
	// ��radֵ���ҹ۲�
	void RotateY(float rad);


};
