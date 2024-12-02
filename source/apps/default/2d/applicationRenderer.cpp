#include "./apps/default/2d/applicationRenderer.h"

namespace JCAT {
    struct PushConstantData {
        glm::mat2 transform { 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    ApplicationRenderer::ApplicationRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass) : device{d}, resourceManager{r} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    ApplicationRenderer::~ApplicationRenderer() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void ApplicationRenderer::createPipelineLayout() {
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

    void ApplicationRenderer::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        pipeline = std::make_unique<GraphicsPipeline>(device, resourceManager, "../shaders/simpleShader2D.vert.spv", "../shaders/simpleShader2D.frag.spv");
        
        std::unordered_map<GraphicsPipeline::PipelineType, PipelineConfigInfo> pipelineConfigs = {};
        pipeline->configurePipelines(pipelineConfigs);
        pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE].renderPass = renderPass;
        pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE].pipelineLayout = pipelineLayout;

        pipeline->createSolidObjectPipeline("../shaders/simpleShader2D.vert.spv", "../shaders/simpleShader2D.frag.spv", pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE]);
        
        std::cout << "Created Pipeline Successfully!" << std::endl;
    }

    void ApplicationRenderer::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameSprite>& gameSprites, const Camera2D& camera) {
        pipeline->bindPipeline(commandBuffer, GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE);

        glm::mat3 projectionView = camera.getProjection() * camera.getView();

        for (GameSprite& spr : gameSprites) {
            spr.transform.rotation = glm::mod(spr.transform.rotation + 0.001f, glm::two_pi<float>());

            PushConstantData push{};
            push.offset = spr.transform.translation;
            push.color = spr.color;
            push.transform = projectionView * spr.transform.transformationMatrix();

            vkCmdPushConstants(commandBuffer, 
                               pipelineLayout, 
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                               0, 
                               sizeof(PushConstantData), 
                               &push);

            spr.model2D->bind(commandBuffer);
            spr.model2D->draw(commandBuffer);
        }
    }
};