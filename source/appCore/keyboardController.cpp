#include "./appCore/keyboardController.h"

namespace JCAT {
    KeyboardController::KeyboardController() {}
    KeyboardController::~KeyboardController() {}

    void KeyboardController::setMoveSpeed(const float newSpeed) {
        moveSpeed = newSpeed;
    }

    void KeyboardController::setSensitivity(const float newSensitivity) {
        sensitivity = newSensitivity;
    }

    void KeyboardController::moveSprite(GLFWwindow* window, float dt, GameSprite& gameSprite) {
        glm::vec2 moveDir{ 0.0f };

        if (glfwGetKey(window, keys2D.moveUp) == GLFW_PRESS) {
            moveDir.y += 1.f;
        }
        if (glfwGetKey(window, keys2D.moveDown) == GLFW_PRESS) {
            moveDir.y -= 1.f;
        }
        if (glfwGetKey(window, keys2D.moveRight) == GLFW_PRESS) {
            moveDir.x += 1.f;
        }
        if (glfwGetKey(window, keys2D.moveLeft) == GLFW_PRESS) {
            moveDir.x -= 1.f;
        }

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameSprite.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
    }

    void KeyboardController::moveObjectInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float deltaX = static_cast<float>(xpos - lastX);
        float deltaY = static_cast<float>(ypos - lastY);

        lastX = xpos;
        lastY = ypos;

        gameObject.transform.rotation.x -= deltaY * sensitivity;
        gameObject.transform.rotation.y += deltaX * sensitivity;

        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
        const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
        const glm::vec3 upDir{ 0.f, -1.f, 0.f };

        glm::vec3 moveDir{ 0.f };
        if (glfwGetKey(window, keys3D.moveForeward) == GLFW_PRESS) {
            moveDir += forwardDir;
        }
        if (glfwGetKey(window, keys3D.moveBackward) == GLFW_PRESS) {
            moveDir -= forwardDir;
        }
        if (glfwGetKey(window, keys3D.moveRight) == GLFW_PRESS) {
            moveDir += rightDir;
        }
        if (glfwGetKey(window, keys3D.moveLeft) == GLFW_PRESS) {
            moveDir -= rightDir;
        }
        if (glfwGetKey(window, keys3D.moveUp) == GLFW_PRESS) {
            moveDir += upDir;
        }
        if (glfwGetKey(window, keys3D.moveDown) == GLFW_PRESS) {
            moveDir -= upDir;
        }

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }

        // Cursor is hidden by default.
        if (escapeCursor == 0) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // If escape key is pressed, the cursor appears again
        if (glfwGetKey(window, keys3D.escape) == GLFW_PRESS) {
            escapeCursor = 1;
        }
    }
};