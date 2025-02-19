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
                int zoomIn = GLFW_KEY_Q;
                int zoomOut = GLFW_KEY_E;
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

            // Common Keys for both 2D and 3D
            struct KeyMappingsCommon {
                int escape = GLFW_KEY_ESCAPE;
                int fullscreen = GLFW_KEY_F;
            };

            KeyboardController();
            ~KeyboardController();

            void setMoveSpeed(const float newSpeed);
            void setSensitivity(const float newSensitivity);

            void moveSprite(GLFWwindow* window, float dt, GameSprite& gameSprite);
            void moveObjectInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject);

            void escapeFunctionality(GLFWwindow* window);
            void fullscreenFunctionality(GLFWwindow* window);

            KeyMappings2D keys2D{};
            KeyMappings3D keys3D{};
            KeyMappingsCommon keysCommon{};

            float lastX = 0.0f;
            float lastY = 0.0f;

            int escapeCursor = 0;
            bool inFullscreen = false;

            // Change these values to change the speed of the WASD movement and look sensitivity respectively
            float moveSpeed{ 3.f };
            float zoomSpeed{ 25.f };
            float sensitivity{ 0.0015f };
        private:
            bool escapeKeyPressedLastFrame = false;
            bool leftMouseButtonPressedLastFrame = false;
            bool fKeyPressedLastFrame = false;
    };
};

#endif