#include "./engine/renderer.h"

namespace JCAT {
    Renderer::Renderer(Window& w, DeviceSetup& d, ResourceManager& r, std::string gameType, bool v) 
    : device{ d }, resourceManager{ r }, window{ w } {
        type = gameType;
        vsync = v;

        recreateSwapChain();
        createCommandBuffers();
    }

    Renderer::~Renderer() {
        freeCommandBuffers();
    }

    void Renderer::recreateSwapChain() {
        VkExtent2D extent = window.getWindowExtent();

        while (extent.width == 0 || extent.height == 0) {
            extent = window.getWindowExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device());

        if (swapChain == nullptr) {
            swapChain = std::make_unique<SwapChain>(device, resourceManager, extent, type, vsync);
        }
        else {
            std::shared_ptr<SwapChain> previousSwapChain = std::move(swapChain);
            swapChain = std::make_unique<SwapChain>(device, resourceManager, extent, previousSwapChain);

            // Must have the same format as the previous swap chain!
            // This means that we cannot switch from a 2D swap chain to a 3D swap chain while the application is running and vice versa!
            if (!previousSwapChain->compareSwapFormats(*swapChain.get())) {
                throw std::runtime_error("Swap chain image (or depth) format has changed!");
            }
        }
    }

    void Renderer::createCommandBuffers() {
        commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers for renderer!");
        }
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    bool Renderer::isFrameInProgress() {
        return isFrameStarted;
    }

    VkCommandBuffer Renderer::beginFrame() {
        
    }

    void Renderer::endFrame() {

    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {

    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {

    }
}