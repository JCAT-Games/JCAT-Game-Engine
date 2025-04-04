#include <array>

#include "./engine/renderer.h"

namespace JCAT {
    /// @brief Constructs a Renderer object.
    /// @param w Reference to the window.
    /// @param d Reference to the device setup.
    /// @param r Reference to the resource manager.
    /// @param gameType The type of game being rendered.
    /// @param v Whether or not to use VSync.
    Renderer::Renderer(Window& w, DeviceSetup& d, ResourceManager& r, std::string gameType, bool v) 
    : device{ d }, resourceManager{ r }, window{ w } {
        type = gameType;
        vsync = v;

        recreateSwapChain();
        createCommandBuffers();

        std::cout << "Initialized Renderer!" << std::endl;
    }

    /// @brief Destructor that cleans up command buffers and swap chain.
    Renderer::~Renderer() {
        freeCommandBuffers();
    }

    /// @brief Recreates the swap chain if the window has been resized.
    /// @throws std::runtime_error if the swap chain image format has changed.
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
            swapChain = std::make_unique<SwapChain>(device, resourceManager, extent, type, vsync, previousSwapChain);

            // Must have the same format as the previous swap chain!
            // This means that we cannot switch from a 2D swap chain to a 3D swap chain while the application is running and vice versa!
            if (!previousSwapChain->compareSwapFormats(*swapChain.get())) {
                throw std::runtime_error("Swap chain image (or depth) format has changed!");
            }
        }
    }

    /// @brief Creates the command buffers for the swap chain.
    /// @throws std::runtime_error if the command buffers fail to allocate.
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

        std::cout << "Created command buffers!" << std::endl;
    }

    /// @brief Frees the command buffers.
    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    /// @brief Renders the scene.
    VkRenderPass Renderer::getSwapChainrenderPass() {
        return swapChain->getRenderPass();
    }

    /// @brief Checks if a frame is currently in progress.
    /// @return True if a frame is in progress, false otherwise.
    bool Renderer::isFrameInProgress() {
        return isFrameStarted;
    }

    /// @brief Retrieves the aspect ratio of the swap chain.
    /// @return The aspect ratio of the swap chain.
    float Renderer::getAspectRatio() const {
        return swapChain->extentAspectRatio();
    }

    /// @brief Retrieves the current command buffer.
    /// @return The current command buffer.
    VkCommandBuffer Renderer::getCurrentCommandBuffer() const {
        assert(isFrameStarted && "Cannot get command buffer when a frame is not in progress!");

        return commandBuffers[currentFrameIndex];
    }

    /// @brief Begins recording a frame.
    /// @return The command buffer for the frame.
    /// @throws std::runtime_error if the command buffer fails to begin recording.
    /// @throws std::runtime_error if the swap chain image fails to acquire.
    VkCommandBuffer Renderer::beginRecordingFrame() {
        assert(!isFrameStarted && "Cannot begin recording frame while a frame is already in progress!");

        VkResult result = swapChain->acquireNextImage(&currentImageIndex);

        // If the window has been resized
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // Swap chain must be recreated
            recreateSwapChain();
            return nullptr;
        }

        // If acquiring the next image fails
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire the next swap chain image!");
        }

        isFrameStarted = true;

        VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Command buffer failed to begin recording!");
        }

        return commandBuffer;
    }

    /// @brief Ends recording a frame.
    /// @throws std::runtime_error if the command buffer fails to end recording.
    /// @throws std::runtime_error if the swap chain image fails to present.
    void Renderer::endRecordingFrame() {
        assert(isFrameStarted && "Can not end recording frame while a frame is not in progress!");

        VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Command buffer failed to end recording!");
        }

        // Submit the command buffers to the GPU once they are done being recorded
        VkResult result = swapChain->submitSwapChainCommandBuffers(&commandBuffer, &currentImageIndex);

        // Check if window has been resized or if there was an error
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
            window.resetWindowResized();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    /// @brief Begins the render pass for the swap chain.
    /// @param commandBuffer The command buffer to begin the render pass on.
    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Cannot begin render pass while frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame!");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

        if (type == "3D") {
            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();
        }
        else {
            std::array<VkClearValue, 1> clearValues{};
            clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();
        }

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Create viewport
        VkViewport viewport{};
        // Make it centered
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        // Make it the size of the swap chain frame extent
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        // Depth goes from 0 to 1 in 3D and just 1 in 2D
        if (type == "3D") {
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
        }
        else {
            viewport.maxDepth = 1.0f;
            viewport.minDepth = 1.0f;
        }
        // Scissor is the same position and size
        VkRect2D scissor{ {0, 0}, swapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Cannot end render pass while frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame!");

        vkCmdEndRenderPass(commandBuffer);
    }

    int Renderer::getFrameIndex() {
        return currentFrameIndex;
    }
}