#include "./apps/default/2d/applicationRenderer.h"

namespace JCAT {
    ApplicationRenderer::ApplicationRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass) : device{d}, resourceManager{r} {
        
    }

    ApplicationRenderer::~ApplicationRenderer() {

    }

    void ApplicationRenderer::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameSprite>& gameSprites, const Camera2D& camera) {

    }

    void ApplicationRenderer::createPipelineLayout() {

    }

    void ApplicationRenderer::createPipeline(VkRenderPass renderPass) {

    }
};