#pragma once
	
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Transform
{
public:

    Transform(const glm::vec3& scale, const  glm::vec3& rotation, const  glm::vec3& position);
    Transform() = default;

    // ��ȡ�������ű���
    [[nodiscard]]glm::vec3 GetScale() const;

    // ��ȡ����ŷ����(������)
    // ������Y-X-Z��˳����ת?�Ƿ���������Ҫ����
    [[nodiscard]]glm::vec3 GetRotation() const;


    // ��ȡ����λ��
    [[nodiscard]]glm::vec3 GetPosition() const;


    // ��ȡ�ҷ�����
    [[nodiscard]]glm::vec3 GetRightAxis() const;

    // ��ȡ�Ϸ�����
    [[nodiscard]]glm::vec3 GetUpAxis() const;

    // ��ȡǰ������
    [[nodiscard]]glm::vec3 GetForwardAxis() const;

    // ��ȡ����任����
    [[nodiscard]]glm::mat4x4 GetLocalToWorldMatrix() const;
 
    // ��ȡ������任����
    [[nodiscard]]glm::mat4x4 GetWorldToLocalMatrix() const;


    // ���ö������ű���
    void SetScale(const glm::vec3 & scale);
    // ���ö������ű���
    void SetScale(float x, float y, float z);

    // ���ö���ŷ����(������)
    // ������Y-X-Z��˳����ת�Ƿ���������Ҫ����

    void SetRotation(const  glm::vec3& eulerAnglesInRadian);
    // ���ö���ŷ����(������)
    // ������Y-X-Z��˳����ת�Ƿ���������Ҫ����
    void SetRotation(float x, float y, float z);


    // ���ö���λ��
    void SetPosition(const  glm::vec3& position);
    // ���ö���λ��
    void SetPosition(float x, float y, float z);

    // ָ��ŷ������ת
    void Rotate(const  glm::vec3& eulerAnglesInRadian);


    //// ָ����ԭ��Ϊ����������ת
    //void RotateAxis(const  glm::vec3& axis, float radian);
    //// ָ����pointΪ��ת����������ת
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
     glm::vec3  m_Rotation = {0.f,0.f,0.f};                                // ��תŷ����(������)
     glm::vec3  m_Position = {0.f,0.f,0.f};                                // λ��


};

