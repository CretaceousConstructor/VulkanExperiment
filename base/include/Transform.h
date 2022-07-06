#pragma once
	
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Transform
{
public:

    Transform(const glm::vec3& scale, const  glm::vec3& rotation, const  glm::vec3& position);
    Transform() = default;

    // 获取对象缩放比例
    [[nodiscard]]glm::vec3 GetScale() const;

    // 获取对象欧拉角(弧度制)
    // 对象以Y-X-Z轴顺序旋转?是否是这样需要测试
    [[nodiscard]]glm::vec3 GetRotation() const;


    // 获取对象位置
    [[nodiscard]]glm::vec3 GetPosition() const;


    // 获取右方向轴
    [[nodiscard]]glm::vec3 GetRightAxis() const;

    // 获取上方向轴
    [[nodiscard]]glm::vec3 GetUpAxis() const;

    // 获取前方向轴
    [[nodiscard]]glm::vec3 GetForwardAxis() const;

    // 获取世界变换矩阵
    [[nodiscard]]glm::mat4x4 GetLocalToWorldMatrix() const;
 
    // 获取逆世界变换矩阵
    [[nodiscard]]glm::mat4x4 GetWorldToLocalMatrix() const;


    // 设置对象缩放比例
    void SetScale(const glm::vec3 & scale);
    // 设置对象缩放比例
    void SetScale(float x, float y, float z);

    // 设置对象欧拉角(弧度制)
    // 对象将以Y-X-Z轴顺序旋转是否是这样需要测试

    void SetRotation(const  glm::vec3& eulerAnglesInRadian);
    // 设置对象欧拉角(弧度制)
    // 对象以Y-X-Z轴顺序旋转是否是这样需要测试
    void SetRotation(float x, float y, float z);


    // 设置对象位置
    void SetPosition(const  glm::vec3& position);
    // 设置对象位置
    void SetPosition(float x, float y, float z);

    // 指定欧拉角旋转
    void Rotate(const  glm::vec3& eulerAnglesInRadian);


    //// 指定以原点为中心绕轴旋转
    //void RotateAxis(const  glm::vec3& axis, float radian);
    //// 指定以point为旋转中心绕轴旋转
    //void RotateAround(const  glm::vec3& point, const  glm::vec3& axis, float radian);

    // 沿着某一方向平移
    void Translate(const  glm::vec3& direction, float magnitude);
    // 观察某一点
    void LookAt(const  glm::vec3& target, const glm::vec3& up = { 0.0f, 1.0f, 0.0f });
    // 沿着某一方向观察
    void LookTo(const  glm::vec3& direction, const  glm::vec3& up = { 0.0f, 1.0f, 0.0f });

private:


private:
     glm::vec3  m_Scale = { 1.0f, 1.0f, 1.0f };                // 缩放
     glm::vec3  m_Rotation = {0.f,0.f,0.f};                                // 旋转欧拉角(弧度制)
     glm::vec3  m_Position = {0.f,0.f,0.f};                                // 位置


};

