#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

namespace JCAT {
    class Camera2D {
        public:
            Camera2D();
            ~Camera2D();

            void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

            void setViewTarget(glm::vec2 position);

            const glm::mat4& getProjection() const;
            const glm::mat4& getView() const;
        private:
            glm::mat4 projectionMatrix{ 1.0f };
            glm::mat4 viewMatrix{ 1.0f };
    };
};  

#endif