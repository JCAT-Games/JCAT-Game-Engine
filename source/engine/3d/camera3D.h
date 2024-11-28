#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

namespace JCAT {
    class Camera3D {
        public:
            Camera3D();
            ~Camera3D();

            void setPerspectiveProjection(float fovy, float aspect, float near, float far);

            void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
            void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
            void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

            const glm::mat4& getProjection() const;
            const glm::mat4& getView() const;
        private:
            glm::mat4 projectionMatrix{ 1.0f };
            glm::mat4 viewMatrix{ 1.0f };
    };
};

#endif