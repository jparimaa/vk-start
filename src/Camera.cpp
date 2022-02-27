#include "Camera.hpp"
#include "Utils.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

Camera::Camera()
{
    updateViewMatrix();
    float fov = 45.0f;
    float ratio = static_cast<float>(c_windowWidth) / static_cast<float>(c_windowHeight);
    float nearClipDistance = 0.1f;
    float farClipDistance = 100.0f;
    m_projectionMatrix = glm::perspective(fov, ratio, nearClipDistance, farClipDistance);
}

void Camera::setPosition(const glm::vec3& pos)
{
    m_position = pos;
    updateViewMatrix();
}

void Camera::setRotation(const glm::vec3& rot)
{
    m_rotation = rot;
    updateViewMatrix();
}

void Camera::translate(const glm::vec3& translation)
{
    m_position += translation;
    updateViewMatrix();
}

void Camera::rotate(const glm::vec3& axis, float amount)
{
    m_rotation += axis * amount;
    updateViewMatrix();
}

const glm::mat4x4& Camera::getViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4x4& Camera::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

void Camera::updateViewMatrix()
{
    const glm::vec4 forward4Comp = glm::yawPitchRoll(m_rotation.y, m_rotation.x, m_rotation.z) * c_forwardZero;
    const glm::vec3 forward(forward4Comp.x, forward4Comp.y, forward4Comp.z);
    m_viewMatrix = glm::lookAt(m_position, m_position + forward, c_up);
}
