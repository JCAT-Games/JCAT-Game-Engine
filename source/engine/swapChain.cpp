#include "./engine/swapChain.h"

#include <limits>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <limits>

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
        for (VkImageView imageView : swapChainImageViews) {
            vkDestroyImageView(device.device(), imageView, nullptr);
        }

        swapChainImageViews.clear();

        if (swapChain != nullptr) {
            vkDestroySwapchainKHR(device.device(), swapChain, nullptr);
            swapChain = nullptr;
        }

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

        // Determine the number of images in the Swap Chain
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = device.windowSurface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = swapExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        // Will need to change later to allow transparency through alpha channels.
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        // Handle the previous swap chain
        createInfo.oldSwapchain = previousSwapChain == nullptr ? VK_NULL_HANDLE : previousSwapChain->swapChain;

        if (vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the swap chain!");
        }

        // Retrieve swap chain images
        vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = swapExtent;
    }

    void SwapChain::createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = swapChainImages[i];

            // Each frame will always represent a 2D image that is being rendered onto the screen
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = swapChainImageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device.device(), &viewInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create swap chain image view!");
            }
        }
    }

    void SwapChain::createDepthResources() {

    }

    void SwapChain::createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;

        // Make this customizable!
        if (type == "3D") {
            colorAttachment.samples = VK_SAMPLE_COUNT_4_BIT;
        }
        else {
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        }

        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // Might need to change for post processing features
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        if (type == "3D") {
            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = findSupportedDepthFormat();
            // THIS SHOULD BE CUSTOMIZABLE
            depthAttachment.samples = VK_SAMPLE_COUNT_4_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            // Will need to change for shadow mapping and post processing
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // Should change later
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // Might need to change for post processing
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }
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

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkFormat SwapChain::findSupportedDepthFormat() {
        std::vector<VkFormat> preferredFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
        VkFormat foundFormat = device.findSupportedDepthFormat(preferredFormats, 
                                                               VK_IMAGE_TILING_OPTIMAL, 
                                                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        swapChainDepthFormat = foundFormat;

        return foundFormat;
    }
}