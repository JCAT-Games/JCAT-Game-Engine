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
            ApplicationRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass);
            ~ApplicationRenderer();

            ApplicationRenderer(const ApplicationRenderer&) = delete;
            ApplicationRenderer& operator=(const ApplicationRenderer&) = delete;

            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameSprite>& gameSprites, const Camera2D& camera);
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