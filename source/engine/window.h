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

            // Creates the window surface
            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        private:
            // Initializes the window
            void initializeWindow();

            // Dimensions of the window
            const int WIDTH;
            const int HEIGHT;

            // Window title
            std::string window_title;

            // Pointer to the GLFW window object
            GLFWwindow* window;
    };
}

#endif