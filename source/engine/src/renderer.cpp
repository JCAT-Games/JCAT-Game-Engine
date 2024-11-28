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

    void Renderer::freeCommandBuffers() {

    }
}