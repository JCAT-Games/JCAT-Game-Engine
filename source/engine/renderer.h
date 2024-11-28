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
            Renderer(Window& w, DeviceSetup& d, ResourceManager& r, std::string type, bool vsync);
            ~Renderer();

            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            bool isFrameInProgress();

            VkCommandBuffer beginFrame();
            void endFrame();

            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            std::string gameType;
            bool vsync;

            Window& window;
            DeviceSetup& device;
            ResourceManager& resourceManager;

            std::unique_ptr<SwapChain> swapChain;
            std::vector<VkCommandBuffer> commandBuffer;

            bool isFrameStarted;
    };
}

#endif //RENDERER_H