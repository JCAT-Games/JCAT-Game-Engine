#include "./engine/window.h"

#include <stdexcept>

namespace JCAT {
    /// @brief Constructs a Window object.
    /// @param w The width of the window.
    /// @param h The height of the window.
    /// @param title The title of the window.
    /// @param f Whether or not the window is fullscreen.
    Window::Window(int w, int h, std::string title, bool f) : width(w), height(h) {
        window_title = title;   
        fullscreen = f;

        initializeWindow();
    }

    /// @brief Destructor that cleans up the window.
    Window::~Window() {
        if (window) {
            glfwDestroyWindow(window);
        }

        glfwTerminate();
    }

    /// @brief Initializes the window.
    /// @throws std::runtime_error if the window fails to initialize.
    void Window::initializeWindow() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        if (fullscreen == false) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

            window = glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);
        }
        else {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            // Make the window the size of the monitor
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            window = glfwCreateWindow(mode->width, mode->height, window_title.c_str(), monitor, nullptr);
        }

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

    GLFWwindow* Window::getWindow() const {
        return window;
    }

    bool Window::windowInFullscreen() const {
        return fullscreen;
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

    /// @brief Creates a window surface for the Vulkan instance.
    /// @param instance The Vulkan instance.
    /// @param surface The surface to create.
    /// @throws std::runtime_error If the window surface creation fails.
    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the window surface!");
        }
    }

    /// @brief Callback function for when the window is resized.
    /// @param w The window that was resized.
    /// @param width The new width of the window.
    /// @param height The new height of the window.
    void Window::framebufferResizeCallback(GLFWwindow* w, int width, int height) {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(w));

        window->windowResized = true;
        window->width = width;
        window->height = height;
    }
}