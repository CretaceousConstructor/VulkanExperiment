#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "Transform.h"
class Camera
{
public:
    Camera() = default;
    virtual ~Camera() ;
    


    //
    // ��ȡ�����λ��
    //


    glm::vec3 GetPosition() const;

    //
    // ��ȡ�������ת
    //

    // ��ȡ��X����ת��ŷ���ǻ���
    float GetRotationX() const;
    // ��ȡ��Y����ת��ŷ���ǻ���
    float GetRotationY() const;

    //
    // ��ȡ�����������������
    //
    glm::vec3 GetRightAxis() const;

    glm::vec3 GetUpAxis() const;
    
    glm::vec3 GetLookAxis() const;

    //
    // ��ȡ����
    //

    glm::mat4x4 GetView() const;
    glm::mat4x4 GetProj() const;
    glm::mat4x4 GetViewProj() const;




    // ��ȡ�ӿ�
    VkViewport GetViewPort() const;


    // ������׶��
    void SetFrustum(float fovY, float aspect, float nearZ, float farZ);


    float GetNearZ() const; 
    float GetFarZ() const; 



    // �����ӿ�
    void SetViewPort(const VkViewport& viewPort);
    void SetViewPort(float X, float Y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

protected:

    // ������ı任
    Transform m_Transform = {};

    // ��׶������
    float m_NearZ = 0.0f;
    float m_FarZ = 0.0f;
    float m_Aspect = 0.0f;
    float m_FovY = 0.0f;

    // ��ǰ�ӿ�
    VkViewport m_ViewPort = {};

};

