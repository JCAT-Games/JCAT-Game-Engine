#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace JCAT {
    /**
     * @class Window
     * @brief Represents a window for the JCAT Game Engine to display content to users
     * 
     * This class contains the setup and management of a display window for the game engine
     * to use in displaying content to users. It includes the dimensions and other information
     * of the display window and is the basis for outputting information to users. Any graphics
     * made with the game engine will be displayed using the GLFWwindow and Window objects. 
     */
    class Window {
        public:
            /**
             * Constructs a Window instance for displaying game engine to user
             * 
             * @param width Window width in pixels
             * @param height Window height in pixels
             * @param title Title of displayed window
             * @param f Whether window should display in fullscreeen
             */
            Window(int width, int height, std::string title, bool f);

            // Destroys the Window instance and releases associated resources from memory */
            ~Window();

            // Make sure an instance of this class CANNOT be copied
            Window(const Window &) = delete;
            Window& operator=(const Window&) = delete;

            /**
             * Ensures that window does not automatically close
             * 
             * @returns true if window should close, false otherwise
             */
            bool shouldWindowClose();

            /// @returns VkExtent2D instance of current window dimensions (width, height)
            VkExtent2D getWindowExtent();

            /// @returns true if window was recently resized, false otherwise
            bool wasWindowResized();

            // Sets windowResized variable to false 
            void resetWindowResized();

            /// @returns Reference to GLFWwindow object used by Window instance
            GLFWwindow* getWindow() const;

            /// @returns true if window is presently in fullscreen mode, false if it is not
            bool windowInFullscreen() const;

            /** 
             * Creates a window surface for the Vulkan instance
             * 
             * @param instance The Vulkan instance
             * @param surface The surface to create
             * @throws std::runtime_error If the window surface creation fails.
            */
            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        private:
            /**
             * Used to resize the the given GLFWwindow by glfwSetFramebufferSizeCallback()
             * 
             * @param w The window that was resized.
             * @param width The new width of the window.
             * @param height The new height of the window.
             */
            static void framebufferResizeCallback(GLFWwindow* w, int width, int height);

            /**
             * @brief Initializes the window.
             * @throws std::runtime_error if the window fails to initialize.
             */
            void initializeWindow();

            // Current width of the window
            int width;
            // Current height of the window
            int height;
            // Whether the window was recently resized (true for yes, false for no)
            bool windowResized = false;
            // If the window is in fullscreen (true for yes, false for no)
            bool fullscreen;

            // Title of window
            std::string window_title;

            // Pointer to the GLFW window object
            GLFWwindow* window;
    };
}; //JCAT

#endif //WINDOW_H