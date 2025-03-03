#ifndef APPLICATION_3D_RENDERER
#define APPLICATION_3D_RENDERER

#include <memory>
#include <vector>

#include "./engine/3d/camera3D.h"
#include "./engine/graphicsPipeline.h"
#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"
#include "./engine/3d/gameObject.h"

namespace JCAT {
    class Application3DRenderer {
        public:
            Application3DRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass);
            ~Application3DRenderer();

            Application3DRenderer(const Application3DRenderer&) = delete;
            Application3DRenderer& operator=(const Application3DRenderer&) = delete;

            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera3D& camera);
        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            DeviceSetup& device;
            ResourceManager& resourceManager;

            std::unique_ptr<GraphicsPipeline> pipeline;
            VkPipelineLayout pipelineLayout;
    };
};

#endif