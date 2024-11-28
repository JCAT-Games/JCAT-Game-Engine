#include "./engine/window.h"

#include <stdexcept>

namespace JCAT {
    Window::Window(int w, int h, std::string title) : width(w), height(h) {
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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);

        if (!window) {
            throw std::runtime_error("Failed to initialize GLFW Window!");
            glfwTerminate();
        }

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    bool Window::shouldWindowClose() {
        return glfwWindowShouldClose(window);
    }

    VkExtent2D Window::getWindowExtent() {
        return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    }

    bool Window::wasWindowResized() {
        return windowResized;
    }

    void Window::resetWindowResized() {
        windowResized = false;
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the window surface!");
        }
    }

    void Window::framebufferResizeCallback(GLFWwindow* w, int width, int height) {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(w));

        window->windowResized = true;
        window->width = width;
        window->height = height;
    }
}