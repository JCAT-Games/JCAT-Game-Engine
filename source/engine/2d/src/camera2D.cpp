#include <cassert>
#include <limits>

#include "./engine/2d/camera2D.h"

namespace JCAT {
    Camera2D::Camera2D() {}
    Camera2D::~Camera2D() {}

    void Camera2D::setOrthographicProjection(float left, float right, float top, float bottom) {
        projectionMatrix = glm::mat3(1.0f);
        projectionMatrix[0][0] = 2.f / (right - left);
        projectionMatrix[1][1] = 2.f / (top - bottom);
        projectionMatrix[2][0] = -(right + left) / (right - left);
        projectionMatrix[2][1] = -(top + bottom) / (top - bottom);
    }

    void Camera2D::setViewTarget(glm::vec2 position) {
        viewMatrix = glm::mat3{1.0f};

        viewMatrix[2][0] = -position.x;
        viewMatrix[2][1] = -position.y;
    }

    const glm::mat3& Camera2D::getProjection() const {
        return projectionMatrix;
    }

    const glm::mat3& Camera2D::getView() const {
        return viewMatrix;
    }
};