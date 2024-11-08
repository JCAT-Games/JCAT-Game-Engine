#include "./engine/swapChain.h"

#include <limits>
#include <algorithm>
#include <cstring>
#include <iostream>

namespace JCAT {
    SwapChain::SwapChain(DeviceSetup& d, VkExtent2D wE, std::string& gameType, bool v) 
    : device{ d }, windowExtent{ wE } {
        type = gameType;
        vsyncEnabled = v;

        init();
    }

    SwapChain::SwapChain(DeviceSetup& d, VkExtent2D wE, std::shared_ptr<SwapChain> previousFrame) 
    : device{ d }, windowExtent{ wE }, previousSwapChain{ previousFrame } {
        init();

        previousSwapChain = nullptr;
    }

    SwapChain::~SwapChain() {

    }

    void SwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }

    bool SwapChain::getVSync() {
        return vsyncEnabled;
    }

    void SwapChain::setVSync(bool toggle) {
        vsyncEnabled = toggle;
    }

    VkResult SwapChain::acquireNextImage(uint32_t* imageIndex) {

    }

    VkResult SwapChain::submitSwapChainCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex) {

    }

    void SwapChain::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D swapExtent = chooseSwapExtent(swapChainSupport.capabilities);
    }

    void SwapChain::createImageViews() {

    }

    void SwapChain::createDepthResources() {

    }

    void SwapChain::createRenderPass() {

    }

    void SwapChain::createFramebuffers() {

    }

    void SwapChain::createSyncObjects() {

    }

    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        if (vsyncEnabled) {
            std::cout << "Refresh Mode: V-Sync" << std::endl;

            for (const VkPresentModeKHR& availiblePresentMode : availablePresentModes) {
                if (availiblePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    std::cout << "Present Mode: Mailbox" << std::endl;
                    return availiblePresentMode;
                }
            }

            std::cout << "Present Mode: FIFO" << std::endl;
            return VK_PRESENT_MODE_FIFO_KHR;
        }
        else {
            for (const VkPresentModeKHR& availiblePresentMode : availablePresentModes) {
                if (availiblePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                    std::cout << "Refresh Mode: Immediate" << std::endl;
                    std::cout << "Present Mode: Immediate" << std::endl;
                    return availiblePresentMode;
                }
            }

            std::cout << "Immediate refresh not availible!" << std::endl;
            std::cout << "Refresh Mode: V-Sync" << std::endl;
            std::cout << "Present Mode: FIFO" << std::endl;
            return VK_PRESENT_MODE_FIFO_KHR;
        }
    }

    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            VkExtent2D actualExtent = windowExtent;

            actualExtent.width = std::max(capabilities.minImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height,
                std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
}