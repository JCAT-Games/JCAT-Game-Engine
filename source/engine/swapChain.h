#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"

namespace JCAT {
    class SwapChain {
        public:
            // The number of frames that can be processed in parallel. 3 is a good number to avoid bottlenecking.
            static constexpr int MAX_FRAMES_IN_FLIGHT = 3;

            SwapChain(DeviceSetup& d, ResourceManager& r, VkExtent2D wE, std::string& gameType, bool v);
            SwapChain(DeviceSetup& d, ResourceManager& r, VkExtent2D wE, std::shared_ptr<SwapChain> previousFrame);
            ~SwapChain();

            SwapChain(const SwapChain&) = delete;
            SwapChain& operator=(const SwapChain&) = delete;

            bool getVSync();
            void setVSync(bool toggle);

            VkResult acquireNextImage(uint32_t* imageIndex);
            VkResult submitSwapChainCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);
            VkResult presentImage(uint32_t* imageIndex);

            bool compareSwapFormats(const SwapChain& otherSwapChain);
        private:
            void init();
            void createSwapChain();
            void createImageViews();
            void createDepthResources();
            void createRenderPass();
            void createFramebuffers();
            void createSynchronizationObjects();

            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
            VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
            VkFormat findSupportedDepthFormat();

            std::string type;
            bool vsyncEnabled;

            DeviceSetup& device;
            ResourceManager& resourceManager;
            VkExtent2D windowExtent;

            VkSwapchainKHR swapChain;
            std::shared_ptr<SwapChain> previousSwapChain;

            std::vector<VkImage> swapChainImages;
            std::vector<VkImageView> swapChainImageViews;

            VkFormat swapChainImageFormat;
            VkFormat swapChainDepthFormat;
            VkExtent2D swapChainExtent;

            std::vector<VkFramebuffer> swapChainFramebuffers;
            VkRenderPass renderPass;

            std::vector<VkImage> depthImages;
            std::vector<VkDeviceMemory> depthImageMemorys;
            std::vector<VkImageView> depthImageViews;

            std::vector<VkSemaphore> imageAvailableSemaphores;
            std::vector<VkSemaphore> renderFinishedSemaphores;
            std::vector<VkFence> inFlightFences;
            std::vector<VkFence> imagesInFlight;
            size_t currentFrame = 0;
    };
}

#endif //SWAP_CHAIN_H