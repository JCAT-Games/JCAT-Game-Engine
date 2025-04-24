#ifndef APPLICATION_3D_RENDERER
#define APPLICATION_3D_RENDERER

#include <memory>
#include <vector>

#include "./engine/3d/camera3D.h"
#include "./engine/graphicsPipeline.h"
#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"
#include "./engine/3d/gameObject.h"
#include "./engine/frameInfo.h"

namespace JCAT {
    class Application3DRenderer {
        public:
            /**
             * Constructs a new 3D Application Renderer object using the given arguments
             * @param d JCAT DeviceSetup object for new application renderer object to use
             * @param r JCAT ResourceManager object for the new application render object to use
             * @param renderPass Vulkan render pass to manage frame buffer and attachment usage
             * @param globalSetLayout The descriptor set layout handle to use in creating pipeline layout
             */
            Application3DRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            
            // Destroys the Vulkan pipeline layout being used by application
            ~Application3DRenderer();

            // Make sure an instance of this class cannot be copied
            Application3DRenderer(const Application3DRenderer&) = delete;
            Application3DRenderer& operator=(const Application3DRenderer&) = delete;

            /**
             * Renders the given game objects into the command buffer of the given FrameInfo
             * @param frameInfo Reference to the JCAT FrameInfo struct to use in rendering game objects
             * @param gameObjects The JCAT game objects to render into the command buffer
             */
            void renderGameObjects(FrameInfo &frameInfo, std::vector<GameObject>& gameObjects);
        private:
            /**
             * Creates the pipeline layout to be used by the 3D Application Renderer object
             * @param globalSetLayout The descriptor set layout handle to use in creating pipeline layout
             */
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

            /**
             * Creates a new solid object graphics pipeline with a unique C++ pointer
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