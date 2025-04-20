#include "./appCore/imguiHandler.h"

namespace JCAT {
    GuiHandler::GuiHandler() {}
    GuiHandler::~GuiHandler() {}

    ImGuiIO& GuiHandler::initializeImGui(Window &window, DeviceSetup &device, Renderer &renderer) {
        // Create ImGui context and set up IO functions
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Setup ImGui style
        ImGui::StyleColorsDark();

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

    void GuiHandler::shutdownImGui() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GuiHandler::check_vk_result(VkResult err) {
        if (err == VK_SUCCESS)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }
}; //JCAT