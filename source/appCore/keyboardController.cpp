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
        glm::vec2 scaleDir{ 1.0f };

        if (glfwGetKey(window, keys2D.moveUp) == GLFW_PRESS) {
            moveDir.y += 1.f;
        }
        if (glfwGetKey(window, keys2D.moveDown) == GLFW_PRESS) {
            moveDir.y -= 1.f;
        }
        if (glfwGetKey(window, keys2D.moveRight) == GLFW_PRESS) {
            moveDir.x -= 1.f;
        }
        if (glfwGetKey(window, keys2D.moveLeft) == GLFW_PRESS) {
            moveDir.x += 1.f;
        }
        if (glfwGetKey(window, keys2D.zoomIn) == GLFW_PRESS){
            //scaleDir.x *= 2.f;
            //scaleDir.y *= 2.f;
            gameSprite.transform.scale.x *= 1.01f;
            gameSprite.transform.scale.y *= 1.01f;
        }
        if (glfwGetKey(window, keys2D.zoomOut) == GLFW_PRESS){
            //scaleDir.x *= 0.5f;
            //scaleDir.y *= 0.5f;
            gameSprite.transform.scale.x *= 0.99f;
            gameSprite.transform.scale.y *= 0.99f;
        }

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameSprite.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
        //if (glm::dot(scaleDir, scaleDir) > std::numeric_limits<float>::epsilon()) {
         //   gameSprite.transform.scale += scaleSpeed * dt * glm::normalize(scaleDir);
        //}

        // Process escape and left click inputs
        escapeFunctionality(window);
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

        // Process escape and left click inputs
        escapeFunctionality(window);
    }

    void KeyboardController::escapeFunctionality(GLFWwindow* window){
        // Escape functionality
        bool isEscapePressed = glfwGetKey(window, keysCommon.escape) == GLFW_PRESS;
        if (isEscapePressed && !escapeKeyPressedLastFrame) {
            escapeCursor++;
            if (escapeCursor >= 2) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        }
        escapeKeyPressedLastFrame = isEscapePressed;

        // Hide cursor when left mouse button pressed
        bool isLeftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if (isLeftMousePressed && !leftMouseButtonPressedLastFrame) {
            if (escapeCursor == 1) {
                escapeCursor = 0; // Reset to hidden state on click
            }
        }
        leftMouseButtonPressedLastFrame = isLeftMousePressed;

        // Update cursor mode based on escapeCursor
        if (escapeCursor == 0) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (escapeCursor == 1) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
};