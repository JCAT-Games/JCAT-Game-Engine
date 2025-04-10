#include <unordered_map>

#include "./apps/default/3d/application3DRenderer.h"

namespace JCAT {
    struct PushConstantData {
        glm::mat4 modelMatrix { 1.0f };
        glm::mat4 normalMatrix { 1.0f };
        uint32_t hasLighting = 0;
        uint32_t hasTexture = 0;
    };

    Application3DRenderer::Application3DRenderer(DeviceSetup& d, ResourceManager& r, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device{d}, resourceManager{r} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    Application3DRenderer::~Application3DRenderer() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void Application3DRenderer::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void Application3DRenderer::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        pipeline = std::make_unique<GraphicsPipeline>(device, resourceManager, "../shaders/simpleShader3D.vert.spv", "../shaders/simpleShader3D.frag.spv");
        
        std::unordered_map<GraphicsPipeline::PipelineType, PipelineConfigInfo> pipelineConfigs = {};
        pipeline->configurePipelines(pipelineConfigs);
        pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE].renderPass = renderPass;
        pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE].pipelineLayout = pipelineLayout;

        pipeline->createSolidObjectPipeline("../shaders/simpleShader3D.vert.spv", "../shaders/simpleShader3D.frag.spv", pipelineConfigs[GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE]);
        
        std::cout << "Created Pipeline Successfully!" << std::endl;
    }

    void Application3DRenderer::renderGameObjects(FrameInfo &frameInfo, std::vector<GameObject>& gameObjects) {
        pipeline->bindPipeline(frameInfo.commandBuffer, GraphicsPipeline::PipelineType::SOLID_OBJECT_PIPELINE);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0, 1, 
            &frameInfo.globalDescriptorSet,
            0, nullptr
        );

        for (GameObject& obj : gameObjects) {
            PushConstantData push{};
            push.modelMatrix = obj.transform.modelMatrix();
            push.normalMatrix = obj.transform.normalMatrix();
            push.hasLighting = obj.hasLighting;
            push.hasTexture = obj.hasTexture;

            vkCmdPushConstants(frameInfo.commandBuffer, 
                               pipelineLayout, 
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                               0, 
                               sizeof(PushConstantData), 
                               &push);

            obj.model3D->bind(frameInfo.commandBuffer);
            obj.model3D->draw(frameInfo.commandBuffer);
        }
    }
};