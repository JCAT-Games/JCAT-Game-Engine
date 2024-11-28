#include <cassert>
#include <limits>

#include "./engine/2d/camera2D.h"

namespace JCAT {
    Camera2D::Camera2D() {}
    Camera2D::~Camera2D() {}

    void Camera2D::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {

    }

    void Camera2D::setViewDirection(glm::vec2 position, float rotation) {

    }

    void Camera2D::setViewTarget(glm::vec2 position, glm::vec2 target) {

    }

    void Camera2D::setViewYX(glm::vec2 position, float yaw) {

    }

    const glm::mat4& Camera2D::getProjection() const {

    }

    const glm::mat4& Camera2D::getView() const {

    }
};