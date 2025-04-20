#ifndef IMGUI_HANDLER
#define IMGUI_HANDLER

#include "./engine/window.h"
#include "./engine/deviceSetup.h"
#include "./engine/renderer.h"

#include "./../imgui/imgui.h"
#include "./../imgui/imgui_impl_glfw.h"
#include "./../imgui/imgui_impl_vulkan.h"

namespace JCAT {
    class GuiHandler {
        public: 
            GuiHandler();
            ~GuiHandler();

            static ImGuiIO& initializeImGui(Window &window, DeviceSetup &device, Renderer &renderer);
            static void shutdownImGui();
            static void check_vk_result(VkResult err);
    };
}; //JCAT

#endif