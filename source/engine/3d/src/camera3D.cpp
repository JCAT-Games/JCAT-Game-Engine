#include <cassert>
#include <limits>

#include "./engine/3D/camera3D.h"

namespace JCAT {
    Camera3D::Camera3D() {}
    Camera3D::~Camera3D() {}

    void Camera3D::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

        const float tanHalfFovy = tan(fovy / 2.f);

        projectionMatrix = glm::mat4{ 0.0f };
        projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
        projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        projectionMatrix[2][2] = far / (far - near);
        projectionMatrix[2][3] = 1.f;
        projectionMatrix[3][2] = -(far * near) / (far - near);
    }

    void Camera3D::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
        const glm::vec3 w{ glm::normalize(direction) };
        const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
        const glm::vec3 v{ glm::cross(w, u) };

        viewMatrix = glm::mat4{ 1.f };
        viewMatrix[0][0] = u.x;
        viewMatrix[1][0] = u.y;
        viewMatrix[2][0] = u.z;
        viewMatrix[0][1] = v.x;
        viewMatrix[1][1] = v.y;
        viewMatrix[2][1] = v.z;
        viewMatrix[0][2] = w.x;
        viewMatrix[1][2] = w.y;
        viewMatrix[2][2] = w.z;
        viewMatrix[3][0] = -glm::dot(u, position);
        viewMatrix[3][1] = -glm::dot(v, position);
        viewMatrix[3][2] = -glm::dot(w, position);
    }

    void Camera3D::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
        setViewDirection(position, target - position, up);
    }

    void Camera3D::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
        const float cz = glm::cos(rotation.z);
        const float sz = glm::sin(rotation.z);
        const float cx = glm::cos(rotation.x);
        const float sx = glm::sin(rotation.x);
        const float cy = glm::cos(rotation.y);
        const float sy = glm::sin(rotation.y);

        const glm::vec3 u{ 
            (cy * cz + sy * sx * sz), 
            (cx * sz), 
            (cy * sx * sz - cz * sy) 
        };

        const glm::vec3 v{ 
            (cz * sy * sx - cy * sz), 
            (cx * cz), 
            (cy * cz * sx + sy * sz) 
        };

        const glm::vec3 w{ 
            (cx * sy), 
            (-sx), 
            (cy * cx) 
        };

        viewMatrix = glm::mat4{ 1.f };
        viewMatrix[0][0] = u.x;
        viewMatrix[1][0] = u.y;
        viewMatrix[2][0] = u.z;
        viewMatrix[0][1] = v.x;
        viewMatrix[1][1] = v.y;
        viewMatrix[2][1] = v.z;
        viewMatrix[0][2] = w.x;
        viewMatrix[1][2] = w.y;
        viewMatrix[2][2] = w.z;
        viewMatrix[3][0] = -glm::dot(u, position);
        viewMatrix[3][1] = -glm::dot(v, position);
        viewMatrix[3][2] = -glm::dot(w, position);
    }

    const glm::mat4& Camera3D::getProjection() const {
        return projectionMatrix;
    }

    const glm::mat4& Camera3D::getView() const {
        return viewMatrix;
    }
};