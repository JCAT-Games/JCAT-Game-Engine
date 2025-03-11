#include "./engine/swapChain.h"

#include <limits>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <array>

namespace JCAT {
    /// @brief Constructs a SwapChain object.
    /// @param d Reference to the device setup.
    /// @param r Reference to the resource manager.
    /// @param wE The window extent.
    /// @param gameType The type of game being rendered.
    /// @param v Whether or not to use VSync.
    SwapChain::SwapChain(DeviceSetup& d, ResourceManager& r, VkExtent2D wE, std::string& gameType, bool v) 
    : device{ d }, resourceManager{ r }, windowExtent{ wE } {
        type = gameType;
        vsyncEnabled = v;

        init();
    }

    /// @brief Constructs a SwapChain object.
    /// @param d Reference to the device setup.
    /// @param r Reference to the resource manager.
    /// @param wE The window extent.
    /// @param gameType The type of game being rendered.
    /// @param v Whether or not to use VSync.
    /// @param previousFrame The previous frame's swap chain.
    SwapChain::SwapChain(DeviceSetup& d, ResourceManager& r, VkExtent2D wE, std::string& gameType, bool v, std::shared_ptr<SwapChain> previousFrame) 
    : device{ d }, resourceManager{ r }, windowExtent{ wE }, previousSwapChain{ previousFrame } {
        type = gameType;
        vsyncEnabled = v;

        init();

        previousSwapChain = nullptr;
    }

    /// @brief Destructor that cleans up swap chain resources.
    SwapChain::~SwapChain() {
        for (VkImageView imageView : swapChainImageViews) {
            vkDestroyImageView(device.device(), imageView, nullptr);
        }

        swapChainImageViews.clear();

        if (swapChain != nullptr) {
            vkDestroySwapchainKHR(device.device(), swapChain, nullptr);
            swapChain = nullptr;
        }

        if (type == "3D") {
            for (int i = 0; i < depthImages.size(); i++) {
                vkDestroyImageView(device.device(), depthImageViews[i], nullptr);
                vkDestroyImage(device.device(), depthImages[i], nullptr);
                vkFreeMemory(device.device(), depthImageMemorys[i], nullptr);
            }
        }

        for (VkFramebuffer framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device.device(), framebuffer, nullptr);
        }

        vkDestroyRenderPass(device.device(), renderPass, nullptr);

        // Cleanup synchronization objects for each frame
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device.device(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device.device(), imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device.device(), inFlightFences[i], nullptr);
        }
    }

    void SwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        
        if (type == "3D") {
            createDepthResources();
        }
        
        createFramebuffers();
        createSynchronizationObjects();
    }

    bool SwapChain::getVSync() {
        return vsyncEnabled;
    }

    VkFramebuffer SwapChain::getFrameBuffer(int index) {
        return swapChainFramebuffers[index];
    }

    VkRenderPass SwapChain::getRenderPass() {
        return renderPass;
    }

    VkExtent2D SwapChain::getSwapChainExtent() {
        return swapChainExtent;
    }

    /// @brief Returns the aspect ratio of the swap chain extent.
    /// @return The aspect ratio of the swap chain extent. 
    float SwapChain::extentAspectRatio() {
        return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }

    void SwapChain::setVSync(bool toggle) {
        vsyncEnabled = toggle;
    }

    /// @brief  Acquires the next image in the swap chain.
    /// @param imageIndex The index of the image to acquire. 
    /// @return The result of acquiring the next image. 
    VkResult SwapChain::acquireNextImage(uint32_t* imageIndex) {
        // Might need to remove this or modify this for preformance later
        vkWaitForFences(device.device(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        VkResult result = vkAcquireNextImageKHR(
            device.device(),
            swapChain,
            std::numeric_limits<uint64_t>::max(),
            imageAvailableSemaphores[currentFrame],  // must be a not signaled semaphore
            VK_NULL_HANDLE,
            imageIndex
        );

        return result;
    }

    /// @brief Submits the swap chain command buffers.
    /// @param buffers The command buffers to submit.
    /// @param imageIndex The index of the image to submit.
    /// @return The result of submitting the swap chain command buffers.
    /// @throw std::runtime_error If the command buffer submission fails.
    VkResult SwapChain::submitSwapChainCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex) {
        // Wait for the previous frame to complete
        if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device.device(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }

        // Mark this current image as in flight
        imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // Wait for the image to become availible before rendering begins
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        // The command buffer for the current frame
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        // Semaphore signal when rendering is complete
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Reset the fence for synchronization
        vkResetFences(device.device(), 1, &inFlightFences[currentFrame]);

        // Submit the draw command buffer
        if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit the draw command buffer!");
        }

        VkResult result = presentImage(imageIndex);

        return result;
    }

    /// @brief Presents the image to the screen.
    /// @param imageIndex The index of the image to present.
    /// @return The result of presenting the image.
    VkResult SwapChain::presentImage(uint32_t* imageIndex) {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];

        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = imageIndex;

        VkResult result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }

    /// @brief Compares the swap chain formats of two swap chains.
    /// @param otherSwapChain The other swap chain to compare.
    bool SwapChain::compareSwapFormats(const SwapChain& otherSwapChain) {
        bool sameFormat = true;

        if (swapChainImageFormat != otherSwapChain.swapChainImageFormat) {
            sameFormat = false;
        }

        if (type == "3D") {
            if (swapChainDepthFormat != otherSwapChain.swapChainDepthFormat) {
                sameFormat = false;
            }
        }

        return sameFormat;
    }


    /// @brief Creates the swap chain.
    /// @throw std::runtime_error If the swap chain creation fails.
    /// @throw std::runtime_error If there is no compatible compisoite alpha mode.
    void SwapChain::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

        //  Print supported surface formats
        std::cout << "Supported Surface Formats:" << std::endl;
        for (const auto& format : swapChainSupport.formats) 
        {
            std::cout << "Format: " << format.format << ", Color Space: " << format.colorSpace << std::endl;
        }

        // Print supported present modes
        std::cout << "Supported Present Modes:" << std::endl;
        for (const auto& mode : swapChainSupport.presentModes) 
        {
            std::cout << "Present Mode: " << mode << std::endl;
        }

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

        // Debug: Print queue family indices
        std::cout << "Graphics Family Index: " << indices.graphicsFamily << std::endl;
        std::cout << "Present Family Index: " << indices.presentFamily << std::endl;

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


        // Choose a supported composite alpha mode
        VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        if (!(swapChainSupport.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)) 
        {
            // try other supported modes
            if (swapChainSupport.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) 
            {
                compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
            } 
            else if (swapChainSupport.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) 
            {
                compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
            } 
            else if (swapChainSupport.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) 
            {
                compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
            } 
            else 
            {
                throw std::runtime_error("No supported composite alpha mode found!");
            }
        }

        // Use the chosen composite alpha mode
        createInfo.compositeAlpha = compositeAlpha;

        // Will need to change later to allow transparency through alpha channels.
        // createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        // Handle the previous swap chain
        createInfo.oldSwapchain = previousSwapChain == nullptr ? VK_NULL_HANDLE : previousSwapChain->swapChain;

        if (vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) 
        {
            // Print more information about the failure
             std::cerr << "Failed to create swap chain! Error code: " << vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain) << std::endl;
            throw std::runtime_error("Failed to create the swap chain!");
        }

        // Retrieve swap chain images
        vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = swapExtent;
    }

    /// @brief Creates the image views for the swap chain.
    /// @throw std::runtime_error If the image view creation fails.
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

    // Used to create depth and stencil images and image views for the 3D swap chain
    /// @brief Creates the depth resources for the swap chain.
    /// @throw std::runtime_error If the depth image view creation fails.
    void SwapChain::createDepthResources() {
        swapChainDepthFormat = findSupportedDepthFormat();
        
        depthImages.resize(swapChainImages.size());
        depthImageMemorys.resize(swapChainImages.size());
        depthImageViews.resize(swapChainImages.size());

        for (int i = 0; i < depthImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = swapChainDepthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            // Might need to change later
            imageInfo.samples = VK_SAMPLE_COUNT_4_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            resourceManager.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = depthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = swapChainDepthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device.device(), &viewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create depth image view!");
            }
        }
    }

    /// @brief Creates the render pass for the swap chain.
    /// @throw std::runtime_error If the render pass creation fails.
    void SwapChain::createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;

        // Make this customizable!
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

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

        VkAttachmentDescription depthAttachment{};
        VkAttachmentReference depthAttachmentRef{};

        if (type == "3D") {
            depthAttachment.format = findSupportedDepthFormat();
            // THIS SHOULD BE CUSTOMIZABLE
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            // Will need to change for shadow mapping and post processing
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // Should change later
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // Might need to change for post processing
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        VkSubpassDescription subpass = {};
        // Will need a different one for phsyics calculation and maybe post processing.
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        if (type == "3D") {
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
        }

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;

        if (type == "3D") {
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else {
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }

        dependency.dstSubpass = 0;

        if (type == "3D") {
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
        else {
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }
        
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        
        if (type == "3D") {
            std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pAttachments = attachments.data();
        }
        else {
            std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pAttachments = attachments.data();
        }

        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    /// @brief Creates the framebuffers for the swap chain.
    /// @throw std::runtime_error If the framebuffer creation fails.
    void SwapChain::createFramebuffers() {
        swapChainFramebuffers.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;

            if (type == "3D") {
                std::array<VkImageView, 2> attachments = { swapChainImageViews[i], depthImageViews[i] };
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
            }
            else {
                framebufferInfo.attachmentCount = 1;
                framebufferInfo.pAttachments = &swapChainImageViews[i];
            }

            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device.device(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }
    }

    /// @brief Creates the synchronization objects for the swap chain.
    /// @throw std::runtime_error If the synchronization object creation fails.
    void SwapChain::createSynchronizationObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device.device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create the necessary synchronization objects for a frame!");
            }
        }
    }

    /// @brief Chooses the swap surface format.
    /// @param availableFormats The available surface formats.
    /// @return The chosen surface format.
    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    /// @brief Chooses the swap present mode.
    /// @param availablePresentModes The available present modes.
    /// @return The chosen present mode.
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

    /// @brief Chooses the swap extent.
    /// @param capabilities The surface capabilities.
    /// @return The chosen swap extent.
    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            VkExtent2D actualExtent = windowExtent;

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

             std::cout << "Chosen Swap Extent: " << actualExtent.width << "x" << actualExtent.height << std::endl;

            return actualExtent;
        }
    }

    /// @brief Finds a supported depth format.
    /// @return The supported depth format.
    VkFormat SwapChain::findSupportedDepthFormat() {
        std::vector<VkFormat> preferredFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
        VkFormat foundFormat = device.findSupportedDepthFormat(preferredFormats, 
                                                               VK_IMAGE_TILING_OPTIMAL, 
                                                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        swapChainDepthFormat = foundFormat;

        return foundFormat;
    }
}