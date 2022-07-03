#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "Transform.h"

class Camera
{
  public:
	Camera()          = default;
	virtual ~Camera() = 0;

	//
	// ��ȡ�����λ��
	[[nodiscard]] glm::vec3 GetPosition() const;
	//�������λ��
	void SetPosition(float x, float y, float z);
	void SetPosition(const glm::vec3 &pos);


	//
	// ��ȡ�������ת
	//
	// ��ȡ��X����ת��ŷ���ǻ���
	[[nodiscard]]float GetRotationX() const;
	// ��ȡ��Y����ת��ŷ���ǻ���
	[[nodiscard]]float GetRotationY() const;

	[[nodiscard]]float GetRotationZ() const;


	//
	// ��ȡ�����������������
	//
	[[nodiscard]]glm::vec3 GetRightAxis() const;
	[[nodiscard]]glm::vec3 GetUpAxis() const;
	[[nodiscard]]glm::vec3 GetLookAxis() const;

	//
	// ��ȡ����
	//
	[[nodiscard]]glm::mat4x4 GetView() const;
	[[nodiscard]]glm::mat4x4 GetProj() const;
	[[nodiscard]]glm::mat4x4 GetViewProj() const;

	// ��ȡ�ӿ�
	VkViewport GetViewPort() const;

	// ������׶��
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	[[nodiscard]]float GetNearZ() const;
	[[nodiscard]]float GetFarZ() const;

	// �����ӿ�
	void SetViewPort(const VkViewport &viewPort);
	void SetViewPort(float X, float Y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

  protected:
	// ������ı任
	Transform m_Transform = {};

	// ��׶������
	float m_NearZ  = 0.0f;
	float m_FarZ   = 0.0f;
	float m_Aspect = 0.0f;
	float m_FovY   = 0.0f;

	// ��ǰ�ӿ�
	VkViewport m_ViewPort = {};
};
