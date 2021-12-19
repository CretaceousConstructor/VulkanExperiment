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
    // 获取摄像机位置
    //


    glm::vec3 GetPosition() const;

    //
    // 获取摄像机旋转
    //

    // 获取绕X轴旋转的欧拉角弧度
    float GetRotationX() const;
    // 获取绕Y轴旋转的欧拉角弧度
    float GetRotationY() const;

    //
    // 获取摄像机的坐标轴向量
    //
    glm::vec3 GetRightAxis() const;

    glm::vec3 GetUpAxis() const;
    
    glm::vec3 GetLookAxis() const;

    //
    // 获取矩阵
    //

    glm::mat4x4 GetView() const;
    glm::mat4x4 GetProj() const;
    glm::mat4x4 GetViewProj() const;




    // 获取视口
    VkViewport GetViewPort() const;


    // 设置视锥体
    void SetFrustum(float fovY, float aspect, float nearZ, float farZ);


    float GetNearZ() const; 
    float GetFarZ() const; 



    // 设置视口
    void SetViewPort(const VkViewport& viewPort);
    void SetViewPort(float X, float Y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

protected:

    // 摄像机的变换
    Transform m_Transform = {};

    // 视锥体属性
    float m_NearZ = 0.0f;
    float m_FarZ = 0.0f;
    float m_Aspect = 0.0f;
    float m_FovY = 0.0f;

    // 当前视口
    VkViewport m_ViewPort = {};

};

