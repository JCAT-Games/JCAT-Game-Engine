#ifndef IMGUI_HANDLER
#define IMGUI_HANDLER

#include "./engine/window.h"
#include "./engine/deviceSetup.h"
#include "./engine/renderer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace JCAT {
    /**
     * @class ImGuiHandler
     * @brief This class contains static functions for simplifying working with ImGui and its
     *        GLFW and Vulkan implementations to implement a UI interface to thIS game engine
     * 
     * @details The Dear ImGui Open Source Library (https://github.com/ocornut/imgui) provides a lot 
     * of functionality for implementing UI (including text, sliders, buttons, and more input options) 
     * into a preexisting application. If working with ImGui here, you may want to read the comments
     * in imgui.h, imgui_impl_vulkan.h, and maybe imgui_impl_glfw.h (all in the repository) beforehand.
     * 
     * https://github.com/ocornut/imgui/tree/master/examples/example_glfw_vulkan
     * This example from the repository was used to help implement many of this features in this class
     */
    class ImGuiHandler {
        public: 
            /**
             * Creates ImGui context & IO configuration and gets ImGui ready to work with Vulkan
             * @param window JCAT Window object being used by current application
             * @param device JCAT DeviceSetup object being used by current application
             * @param renderer JCAT Renderer object being used by current application
             * @param lightStyle Whether to use a "light mode" style in ImGui UI window(s)
             * @return Reference to ImGui IO configuration object to be used by application
             */
            static ImGuiIO& initializeImGui(Window &window, DeviceSetup &device, Renderer &renderer, bool lightStyle = false);

            /// Creates a new ImGui frame for GLFW and Vulkan
            static void startImGuiFrame();

            /**
             * Renders ImGui content and records ImGui primitives to the given command buffer
             * @param commandBuffer Vulkan command buffer to add ImGui content to
             */
            static void renderImGui(VkCommandBuffer &commandBuffer);

            /// Shuts down ImGui processes for GLFW and Vulkan and destroys ImGui context
            static void shutdownImGui();

            /**
             * Vulkan result checker to be used by ImGui Vulkan implementation,
             * If err is not VK_SUCCESS, Prints error message to stderr and aborts application
             * @param VkResult Vulkan result to be checked
             */
            static void check_vk_result(VkResult err);
    };
}; //JCAT

#endif