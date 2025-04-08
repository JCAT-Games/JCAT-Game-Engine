#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"

namespace JCAT {
    /**
     * @class SwapChain
     * @brief Contains presentable images and functions for utilizing them by JCAT Game Engine
     * 
     * This class keeps track of the queues and images to be presented to the screen as
     * well as the synchronization primitives for communicating with prensentation engine.
     * Only one image can be presented at a time, but multiple can be queued for efficiency.
     * Also allows for acquiring the next presentable image and submitting queues of images.
     */
    class SwapChain {
        public:
            // The number of frames that can be processed in parallel. 3 is a good number to avoid bottlenecking.
            static constexpr int MAX_FRAMES_IN_FLIGHT = 3;

            /**
             * what do
             * @param d Reference to device to use in swap chain processes
             * @param r Reference to resource manager to use in swap chain processes
             * @param wE Extent of the window used by the swap chain
             * @param gameType Whether a 2D or 3D game is being used
             * @param v Whether or not to use VSync.
             */
            SwapChain(DeviceSetup& d, ResourceManager& r, VkExtent2D wE, std::string& gameType, bool v);

            /**
             * what do
             * @param d Reference to device to use in swap chain processes
             * @param r Reference to resource manager to use in swap chain processes
             * @param wE Extent of the window used by the swap chain
             * @param gameType Whether a 2D or 3D game is being used
             * @param v Whether or not to use VSync.
             * @param previousFrame Pointer to the previous frame's swap chain
             */
            SwapChain(DeviceSetup& d, ResourceManager& r, VkExtent2D wE, std::string& gameType, bool v, std::shared_ptr<SwapChain> previousFrame);
            
            /** Destructor that cleans up swap chain resources and deallocates memory */
            ~SwapChain();

            // Make sure an instance of this class CANNOT be copied
            SwapChain(const SwapChain&) = delete;
            SwapChain& operator=(const SwapChain&) = delete;

            /// @return true if VSync is currently being used, false otherwise
            bool getVSync();
            
            /// @return The frame buffer located at the specified index
            VkFramebuffer getFrameBuffer(int index);

            /// @return Vulkan render pass currently being used by swap chain
            VkRenderPass getRenderPass();

            /// @return The current extent of the swap chain
            VkExtent2D getSwapChainExtent();

            /// @return The aspect ratio of the swap chain extent as a width/height float
            float extentAspectRatio();

            /// @param toggle Whether VSync is being used (true if yes, false otherwise)
            void setVSync(bool toggle);

            /**
             * Acquires the index of the next available presentable image 
             * @param imageIndex Reference to where returned image should be stored
             * @return Vulkan Result of whether operation was successful or not
             */
            VkResult acquireNextImage(uint32_t* imageIndex);

            /**
             * Resets fences, submits given command buffers, and presents the images
             * @param buffers Vulkan command buffers to submit to be presented
             * @param imageIndex The index of the image to be used in submission
             * @throw std::runtime_error if the command buffer(s) could not be submitted
             * @return Vulkan Result of whether operation was successful or not
             */
            VkResult submitSwapChainCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

            /**
             * Queues image(s) at the given indices for presentation
             * @param imageIndex The index or indices of the image(s) to present
             * @return Vulkan Result of whether operation was successful or not
             */
            VkResult presentImage(uint32_t* imageIndex);

            /**
             * Compares the image formats and depth formats (if applicable) of the two swap chains
             * @param otherSwapChain Reference to the other swap chain object to compare to current one
             * @return true if the format(s) of both swap chains are the same, false otherwise
             */
            bool compareSwapFormats(const SwapChain& otherSwapChain);
        private:
            // Creates all objects for use by the swap chain (depth resources only if type is 3D)
            void init();
            void createSwapChain();
            void createImageViews();
            void createDepthResources();
            void createRenderPass();
            void createFramebuffers();
            void createSynchronizationObjects();

            /// @return The best swap chain surface format from the provided vector "availableFormats"
            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
            /// @return The best swap chain refresh mode and present mode from the given vector "availablePresentModes"
            VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
            /// @return The swap chain extent from either the given "capabilities" or the window's extent
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
            /// @return The best supported depth format for use by swap chain
            VkFormat findSupportedDepthFormat();

            // Whether a 3D or 2D game is currently being rendered
            std::string type;
            // Whether VSync is enabled or not
            bool vsyncEnabled;

            // Reference to the JCAT device being used by swap chain
            DeviceSetup& device;
            // Reference to the JCAT resource manager being used by swap chain
            ResourceManager& resourceManager;
            // The extent (width and height) of the window being used by swap chain
            VkExtent2D windowExtent;

            // The actual Vulkan swap chain object being used
            VkSwapchainKHR swapChain;
            // A pointer to the prior Vulkan swap chain object used
            std::shared_ptr<SwapChain> previousSwapChain;

            // All of the Vulkan image objects used by the swap chain
            std::vector<VkImage> swapChainImages;
            // All of the Vulkan image views (with ranges and other info) used by swap chain
            std::vector<VkImageView> swapChainImageViews;

            // The image format currently used by the swap chain
            VkFormat swapChainImageFormat;
            // The depth format currently used by the swap chain (3D only)
            VkFormat swapChainDepthFormat;
            // The extent (width and height) used by the swap chain
            VkExtent2D swapChainExtent;

            // All of the frame buffers used by the swap chain
            std::vector<VkFramebuffer> swapChainFramebuffers;
            // The Vulkan render pass object (representing attachments, subpasses, and dependencies)
            VkRenderPass renderPass;

            // All of the depth images used by swap chain (3D only)
            std::vector<VkImage> depthImages;
            // The data in device memory being used by depth images (3D only)
            std::vector<VkDeviceMemory> depthImageMemorys;
            // The Vulkan image views corresponding to the depth images (3D only)
            std::vector<VkImageView> depthImageViews;

            // All of the available semaphores (synchronization primitives) for use
            std::vector<VkSemaphore> imageAvailableSemaphores;
            // All of the semaphores (synchronization primitives) that have finished rendering
            std::vector<VkSemaphore> renderFinishedSemaphores;
            // All of the fences (synchronization primitives) that are currently in flight
            std::vector<VkFence> inFlightFences;
            // All of the images that are currently being submitted for rendering
            std::vector<VkFence> imagesInFlight;
            // Index of the current frame being rendered
            size_t currentFrame = 0;
    };
} //JCAT

#endif //SWAP_CHAIN_H