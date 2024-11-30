#include <unordered_map>

#include "./apps/default/3d/application3DRenderer.h"

namespace JCAT {
    struct PushConstantData {
        glm::mat4 transform { 1.0f };
        alignas(16) glm::vec3 color;
    };

    Application3DRenderer::Application3DRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass) : device{d}, resourceManager{r} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    Application3DRenderer::~Application3DRenderer() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void Application3DRenderer::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void Application3DRenderer::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        pipeline = std::make_unique<GraphicsPipeline>(device.device(), resourceManager, "../shaders/simpleShader3D.vert", "../shaders/simpleShader3D.frag");
        
        std::unordered_map<GraphicsPipeline::PipelineType, PipelineConfigInfo> pipelineConfigs = {};
        pipeline->configurePipelines(pipelineConfigs);
        pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE].renderPass = renderPass;
        pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE].pipelineLayout = pipelineLayout;

        pipeline->createSolidObjectPipeline("../shaders/simpleShader3D.vert", "../shaders/simpleShader3D.frag", pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE]);
    }

    void Application3DRenderer::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera3D& camera) {
        pipeline->bindPipeline(commandBuffer, GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE);

        glm::mat4 projectionView = camera.getProjection() * camera.getView();

        for (GameObject& obj : gameObjects) {
            PushConstantData push{};
            push.color = obj.color;
            push.transform = projectionView * obj.transform.transformationMatrix();

            vkCmdPushConstants(commandBuffer, 
                               pipelineLayout, 
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                               0, 
                               sizeof(PushConstantData), 
                               &push);

            obj.model3D->bind(commandBuffer);
            obj.model3D->draw(commandBuffer);
        }
    }
};