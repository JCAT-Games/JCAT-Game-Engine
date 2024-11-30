#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace JCAT {
    class Window {
        public:
            // Constructor
            Window(int width, int height, std::string title);

            // Destructor
            ~Window();

            // Make sure an instance of this class CANNOT be copied
            Window(const Window &) = delete;
            Window& operator=(const Window&) = delete;

            // Prevents the window from automatically closing
            bool shouldWindowClose();
            VkExtent2D getWindowExtent();
            bool wasWindowResized();
            void resetWindowResized();
            GLFWwindow* getWindow() const;

            // Creates the window surface
            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        private:
            static void framebufferResizeCallback(GLFWwindow* w, int width, int height);

            // Initializes the window
            void initializeWindow();

            // Dimensions of the window
            int width;
            int height;
            bool windowResized = false;

            // Window title
            std::string window_title;

            // Pointer to the GLFW window object
            GLFWwindow* window;
    };
};

#endif