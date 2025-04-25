#ifndef APPLICATION_RENDERER
#define APPLICATION_RENDERER

#include <memory>
#include <vector>

#include "./engine/2d/camera2D.h"
#include "./engine/graphicsPipeline.h"
#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"
#include "./engine/2d/gameSprite.h"

namespace JCAT {
    class ApplicationRenderer {
        public:
            /**
             * Constructs a new 2D Application Renderer object using the given arguments
             * @param d JCAT DeviceSetup object for new application renderer object to use
             * @param r JCAT ResourceManager object for the new application render object to use
             * @param renderPass Vulkan render pass to manage frame buffer and attachment usage
             * @param globalSetLayout The descriptor set layout handle to use in creating pipeline layout
             */
            ApplicationRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass);

            // Destroys the Vulkan pipeline layout being used by application
            ~ApplicationRenderer();

            // Make sure an instance of this class cannot be copied
            ApplicationRenderer(const ApplicationRenderer&) = delete;
            ApplicationRenderer& operator=(const ApplicationRenderer&) = delete;

            /**
             * Renders the given game sprites into the command buffer of the given FrameInfo
             * @param commandBuffer The Vulkan command buffer to render the given sprites into
             * @param gameSprites The JCAT game sprites to render into the command buffer
             * @param camera Reference to the JCAT 2D Camera object to obtain current projection view from
             */
            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameSprite>& gameSprites, const Camera2D& camera);
        private:
            // Creates the pipeline layout to be used by the 2D Application Renderer object
            void createPipelineLayout();

            /**
             * Creates a new solid sprite graphics pipeline with a unique C++ pointer
             * @param renderPass Vulkan render pass to manage frame buffer and attachment usage
             */
            void createPipeline(VkRenderPass renderPass);

            DeviceSetup& device;
            ResourceManager& resourceManager;

            std::unique_ptr<GraphicsPipeline> pipeline;
            VkPipelineLayout pipelineLayout;
    };
};

#endif