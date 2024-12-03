#include "./appCore/keyboardController.h"

namespace JCAT {
    KeyboardController::KeyboardController() {}
    KeyboardController::~KeyboardController() {}

    void KeyboardController::setMoveSpeed(const float newSpeed) {
        moveSpeed = newSpeed;
    }

    void KeyboardController::setLookSpeed(const float newSpeed) {
        lookSpeed = newSpeed;
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
        glm::vec3 rotate{0};

        if (glfwGetKey(window, keys3D.lookRight) == GLFW_PRESS) {
            rotate.y += 1.f;
        }
        if (glfwGetKey(window, keys3D.lookLeft) == GLFW_PRESS) {
            rotate.y -= 1.f;
        }
        if (glfwGetKey(window, keys3D.lookUp) == GLFW_PRESS) {
            rotate.x += 1.f;
        }
        if (glfwGetKey(window, keys3D.lookDown) == GLFW_PRESS) {
            rotate.x -= 1.f;
        }

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

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
    }
};