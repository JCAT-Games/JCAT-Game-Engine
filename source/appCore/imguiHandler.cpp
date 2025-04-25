#include "./appCore/imguiHandler.h"

namespace JCAT {

    ImGuiIO& ImGuiHandler::initializeImGui(Window &window, DeviceSetup &device, Renderer &renderer, bool lightStyle) {
        // Create ImGui context and set up IO functions
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Setup ImGui style
        if(lightStyle) ImGui::StyleColorsLight();
        else ImGui::StyleColorsDark();

        // ImGui setup for working with Vulkan
        ImGui_ImplGlfw_InitForVulkan(window.getWindow(), false);
        ImGui_ImplVulkan_InitInfo initInfo {};
        initInfo.Instance = device.getInstance();
        initInfo.PhysicalDevice = device.getPhysicalDevice();
        initInfo.Device = device.device();
        initInfo.QueueFamily = device.findPhysicalQueueFamilies().graphicsFamily;
        initInfo.Queue = device.graphicsQueue();
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.DescriptorPoolSize = 3;
        initInfo.RenderPass = renderer.getSwapChainrenderPass();
        initInfo.Subpass = 0;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = 2;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&initInfo);

        return io;
    }

    void ImGuiHandler::startImGuiFrame() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiHandler::renderImGui(VkCommandBuffer &commandBuffer) {
        // Records ImGui primitives to given command buffer
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }

    void ImGuiHandler::shutdownImGui() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiHandler::check_vk_result(VkResult err) {
        if (err == VK_SUCCESS)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }
}; //JCAT