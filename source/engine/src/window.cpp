#include "./engine/window.h"

#include <stdexcept>

namespace JCAT {
    Window::Window(int width, int height, std::string title) : WIDTH(width), HEIGHT(height) {
        window_title = title;
        initializeWindow();
    }

    Window::~Window() {
        if (window) {
            glfwDestroyWindow(window);
        }

        glfwTerminate();
    }

    void Window::initializeWindow() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, window_title.c_str(), nullptr, nullptr);

        if (!window) {
            throw std::runtime_error("Failed to initialize GLFW Window!");
            glfwTerminate();
        }
    }

    bool Window::shouldWindowClose() {
        return glfwWindowShouldClose(window);
    }

    VkExtent2D Window::getWindowExtent() {
        return { static_cast<uint32_t>(WIDTH), static_cast<uint32_t>(HEIGHT) };
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the window surface!");
        }
    }
}