#include <cassert>
#include <limits>

#include "./engine/2d/camera2D.h"

namespace JCAT {
    Camera2D::Camera2D() {}
    Camera2D::~Camera2D() {}

    void Camera2D::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
        projectionMatrix = glm::mat4{ 1.0f };
        projectionMatrix[0][0] = 2.f / (right - left);
        projectionMatrix[1][1] = 2.f / (bottom - top);
        projectionMatrix[2][2] = 1.f / (far - near);
        projectionMatrix[3][0] = -(right + left) / (right - left);
        projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
        projectionMatrix[3][2] = -near / (far - near);
    }

    void Camera2D::setViewTarget(glm::vec2 position) {
        viewMatrix = glm::mat4{1.0f};

        // Set translation (negative position since we move the world relative to the camera)
        viewMatrix[3][0] = -position.x;
        viewMatrix[3][1] = -position.y;
    }

    const glm::mat4& Camera2D::getProjection() const {
        return projectionMatrix;
    }

    const glm::mat4& Camera2D::getView() const {
        return viewMatrix;
    }
};