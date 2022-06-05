#pragma once
#include "Camera.h"

class FirstPersonCamera : public Camera
{
  public:
	FirstPersonCamera()  = default;
	~FirstPersonCamera() override = default ;

	// 设置摄像机位置Camera position
	//void SetPosition(float x, float y, float z);
	//void SetPosition(const glm::vec3 &pos);
	// 设置摄像机的朝向Camera direction
	void LookAt(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up);
	void LookTo(const glm::vec3 &pos, const glm::vec3 &to, const glm::vec3 &up);

	// 平移
	void Strafe(float d);
	// 直行(平面移动)
	void Walk(float d);
	// 前进(朝前向移动)
	void MoveForward(float d);

	// 上下观察
	// 正rad值向上观察
	// 负rad值向下观察
	void Pitch(float rad);
	// 左右观察
	// 正rad值向左观察
	// 负rad值向右观察
	void RotateY(float rad);


};
