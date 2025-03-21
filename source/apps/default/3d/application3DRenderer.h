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
            Application3DRenderer(
                DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~Application3DRenderer();

            Application3DRenderer(const Application3DRenderer&) = delete;
            Application3DRenderer& operator=(const Application3DRenderer&) = delete;

            void renderGameObjects(FrameInfo &frameInfo, std::vector<GameObject>& gameObjects);
        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);

            DeviceSetup& device;
            ResourceManager& resourceManager;

            std::unique_ptr<GraphicsPipeline> pipeline;
            VkPipelineLayout pipelineLayout;
    };
};

#endif