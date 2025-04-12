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
    /**
     * @class Renderer
     * @brief Main renderer for frames and images used by JCAT Game Engine
     * 
     * This class contains functions for making use of Vulkan command buffers and
     * utilizing them to render frames to display to the game engine user. Also makes
     * use of the game engine swap chain to render frames using command buffers.
     */
    class Renderer {
        public:
            /**
             * Constructs a Renderer object.
             * @param w Reference to the window.
             * @param d Reference to the device setup.
             * @param r Reference to the resource manager.
             * @param gameType The type of game being rendered.
             * @param v Whether or not to use VSync.
             */
            Renderer(Window& w, DeviceSetup& d, ResourceManager& r, std::string gameType, bool v);
            
            /** Destructor that cleans up command buffers and swap chain. */
            ~Renderer();

            // Make sure an instance of this class CANNOT be copied
            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            /** Renders the scene. */
            VkRenderPass getSwapChainrenderPass();

            /// @return true if a frame is in progress, false otherwise.
            bool isFrameInProgress();

            /// @return The aspect ratio of the swap chain (as a float).
            float getAspectRatio() const;

            /// @return The current command buffer.
            VkCommandBuffer getCurrentCommandBuffer() const;

            /**
             * @brief Begins recording a frame.
             * @return The command buffer for the recorded frame.
             * @throws std::runtime_error if the command buffer fails to begin recording.
             * @throws std::runtime_error if the swap chain image fails to acquire.
             */
            VkCommandBuffer beginRecordingFrame();
            
            /**
             * @brief Ends recording a frame and submits swap chain command buffers.
             * @throws std::runtime_error if the command buffer fails to end recording.
             * @throws std::runtime_error if the swap chain image fails to present.
             */
            void endRecordingFrame();

            /** 
             * Begins the render pass for the swap chain and creates viewport.
             * @param commandBuffer The command buffer to begin the render pass on.
             */
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            
            /** 
             * Ends the render pass for the swap chain.
             * @param commandBuffer The command buffer to end the render pass for.
             */
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

            int getFrameIndex();
        private:
            /**
             * @brief Recreates the swap chain if the window has been resized.
             * @throws std::runtime_error if the swap chain image format (2D/3D) has changed.
             */
            void recreateSwapChain();

            /**
             * Creates the command buffers for the swap chain.
             * @throws std::runtime_error if the command buffers fail to allocate.
             */
            void createCommandBuffers();
            
            // Frees all command buffers
            void freeCommandBuffers();

            // Whether the game being rendered is 2D or 3D
            std::string type;
            // Whether VSync is being used
            bool vsync;

            // JCAT Window object for displaying content to
            Window& window;
            // Device being used to run the game engine
            DeviceSetup& device;
            // JCAT Resource Manager that stores command buffers and images
            ResourceManager& resourceManager;

            // Pointer to the swap chain object being used by the game engine
            std::unique_ptr<SwapChain> swapChain;
            // C++ Vector of command buffers
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex = 0;
            int currentFrameIndex = 0;
            bool isFrameStarted;
    };
} //JCAT

#endif //RENDERER_H