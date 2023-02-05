#pragma once
#include "Camera.h"

class FirstPersonCamera : public Camera
{
  public:
	enum class Camera_Movement
	{
		Forward,
		Backward,
		Left,
		Right,
		RollLeft,
		RollRight,
		ZoomIn,
		ZoomOut

	};

	FirstPersonCamera(glm::vec3 position_ = glm::vec3(0.f, 0.f, 0.f));
	FirstPersonCamera(float posX, float posY, float posZ);
	~FirstPersonCamera() override = default;

	// 设置摄像机的朝向Camera direction
	//void LookAt(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up);
	//void LookTo(const glm::vec3 &pos, const glm::vec3 &to, const glm::vec3 &up);

	// 平移
	//void Strafe(float d);
	//// 直行(平面移动)
	//void Walk(float d);
	//// 前进(朝前向移动)
	//void MoveForward(float d);

	[[nodiscard]] glm::mat4 GetViewMatrix() const;
	[[nodiscard]] glm::vec3 GetEyePos() const;
	void                    ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void                    ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void                    ProcessMouseScroll(float scroll_offset);

  private:
	void UpdateCameraVectors();

  private:
	glm::vec3 position;
	glm::quat orientation;

	float yaw_angle;
	float pitch_angle;
	float roll_angle;

	float movement_speed;
	float mouse_sensitivity;

	static constexpr float min_fov = glm::radians(20.f);
	static constexpr float max_fov = glm::radians(100.f);

	// Default camera values
	static constexpr float speed_       = 10.0f;
	static constexpr float sensitivity_ = 1.5f;
	static constexpr float zoom_rate    = 0.0001f;
};
