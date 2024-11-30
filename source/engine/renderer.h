#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

#include "./engine/window.h"
#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"
#include "./engine/swapChain.h"

namespace JCAT {
    class Renderer {
        public:
            Renderer(Window& w, DeviceSetup& d, ResourceManager& r, std::string gameType, bool v);
            ~Renderer();

            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            VkRenderPass getSwapChainrenderPass();

            bool isFrameInProgress();
            float getAspectRatio() const;
            VkCommandBuffer getCurrentCommandBuffer() const;

            VkCommandBuffer beginRecordingFrame();
            void endRecordingFrame();

            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
        private:
            void recreateSwapChain();
            void createCommandBuffers();
            void freeCommandBuffers();

            std::string type;
            bool vsync;

            Window& window;
            DeviceSetup& device;
            ResourceManager& resourceManager;

            std::unique_ptr<SwapChain> swapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex;
            bool isFrameStarted;
    };
}

#endif //RENDERER_H