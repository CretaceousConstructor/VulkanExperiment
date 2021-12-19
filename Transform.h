#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Transform
{
public:
    Transform(const glm::vec3& scale, const  glm::vec3& rotation, const  glm::vec3& position);
    Transform() = default;
    ~Transform() = default;

    Transform(const Transform&) = default;
    Transform& operator=(const Transform&) = default;

    Transform(Transform&&) = default;
    Transform& operator=(Transform&&) = default;

    // ��ȡ�������ű���
    glm::vec3 GetScale() const;

    // ��ȡ����ŷ����(������)
    // ������Z-X-Y��˳����ת
    glm::vec3 GetRotation() const;


    // ��ȡ����λ��
    glm::vec3 GetPosition() const;


    // ��ȡ�ҷ�����
    glm::vec3 GetRightAxis() const;

    // ��ȡ�Ϸ�����
    glm::vec3 GetUpAxis() const;

    // ��ȡǰ������
    glm::vec3 GetForwardAxis() const;

    // ��ȡ����任����
    glm::mat4x4 GetLocalToWorldMatrix() const;
 
    // ��ȡ������任����
    glm::mat4x4 GetWorldToLocalMatrix() const;


    // ���ö������ű���
    void SetScale(const glm::vec3 & scale);
    // ���ö������ű���
    void SetScale(float x, float y, float z);

    // ���ö���ŷ����(������)
    // ������Z-X-Y��˳����ת
    void SetRotation(const  glm::vec3& eulerAnglesInRadian);
    // ���ö���ŷ����(������)
    // ������Z-X-Y��˳����ת
    void SetRotation(float x, float y, float z);

    // ���ö���λ��
    void SetPosition(const  glm::vec3& position);
    // ���ö���λ��
    void SetPosition(float x, float y, float z);

    // ָ��ŷ������ת����
    void Rotate(const  glm::vec3& eulerAnglesInRadian);
    // ָ����ԭ��Ϊ����������ת
    void RotateAxis(const  glm::vec3& axis, float radian);
    // ָ����pointΪ��ת����������ת
    //void RotateAround(const  glm::vec3& point, const  glm::vec3& axis, float radian);

    // ����ĳһ����ƽ��
    void Translate(const  glm::vec3& direction, float magnitude);

    // �۲�ĳһ��
    void LookAt(const  glm::vec3& target, const glm::vec3& up = { 0.0f, 1.0f, 0.0f });
    // ����ĳһ����۲�
    void LookTo(const  glm::vec3& direction, const  glm::vec3& up = { 0.0f, 1.0f, 0.0f });

private:


private:
     glm::vec3  m_Scale = { 1.0f, 1.0f, 1.0f };                // ����
     glm::vec3  m_Rotation = {};                                // ��תŷ����(������)
     glm::vec3  m_Position = {};                                // λ��










};

