#ifndef KEYBOARD_CONTROLLER_H
#define KEYBOARD_CONTROLLER_H

#include "./engine/2d/gameSprite.h"
#include "./engine/3d/gameObject.h"
#include "./engine/window.h"

namespace JCAT {
    class KeyboardController {
        public:
            // For 2D
            struct KeyMappings2D {
                int moveLeft = GLFW_KEY_A;
                int moveRight = GLFW_KEY_D;
                int moveUp = GLFW_KEY_W;
                int moveDown = GLFW_KEY_S;
            };

            // For 3D
            struct KeyMappings3D {
                int moveLeft = GLFW_KEY_A;
                int moveRight = GLFW_KEY_D;
                int moveForeward = GLFW_KEY_W;
                int moveBackward = GLFW_KEY_S;
                int moveUp = GLFW_KEY_E;
                int moveDown = GLFW_KEY_Q;
                int lookLeft = GLFW_KEY_LEFT;
                int lookRight = GLFW_KEY_RIGHT;
                int lookUp = GLFW_KEY_UP;
                int lookDown = GLFW_KEY_DOWN;
            };

            KeyboardController();
            ~KeyboardController();

            void setMoveSpeed(const float newSpeed);
            void setLookSpeed(const float newSpeed);

            void moveSprite(GLFWwindow* window, float dt, GameSprite& gameSprite);
            void moveObjectInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject);

            KeyMappings2D keys2D{};
            KeyMappings3D keys3D{};

            float moveSpeed{ 3.f };
            float lookSpeed{ 1.5f };
        private:
    };
};

#endif