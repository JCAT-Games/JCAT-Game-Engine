#include <cassert>

#include "./engine/graphicsPipeline.h"

namespace JCAT {
    GraphicsPipeline::GraphicsPipeline(DeviceSetup &physicalDevice, ResourceManager &resourceManager, const std::string& vertFilepath, const std::string& fragfilepath, std::unordered_map<PipelineType, PipelineConfigInfo>& configInfos) : device{ physicalDevice }, resources{ resourceManager } {}

    GraphicsPipeline::~GraphicsPipeline() {
        vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);

        for (std::pair<const PipelineType, VkPipeline>& graphicsPipeline : graphicsPipelines) {
            vkDestroyPipeline(device.device(), graphicsPipeline.second, nullptr);
        }
    }

    void GraphicsPipeline::bindPipeline(VkCommandBuffer commandBuffer, PipelineType type) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[type]);
    }

    void GraphicsPipeline::configurePipelines(std::unordered_map<PipelineType, PipelineConfigInfo>& configInfos) {
        std::cout << "Configuring Pipelines" << std::endl;

        configInfos.insert({PipelineType::SOLID_SPRITE_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::TRANSPARENT_SPRITE_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::SOLID_OBJECT_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::TRANSPARENT_OBJECT_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::UI_RENDERING_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::SHADOW_MAPPING_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::SKYBOX_RENDERING_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::PARTICLE_RENDERING_PIPELINE, PipelineConfigInfo{}});
        configInfos.insert({PipelineType::POST_PROCESSING_PIPELINE, PipelineConfigInfo{}});

        // These settings will later on be modifiable
        // Typically, a game will have a "Graphics Settings" section in the menu where these variables can be modified
        for (std::pair<const PipelineType, PipelineConfigInfo>& configInfo : configInfos) {
            configInfo.second.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

            configInfo.second.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            configInfo.second.viewportInfo.viewportCount = 1;
            configInfo.second.viewportInfo.pViewports = nullptr;
            configInfo.second.viewportInfo.scissorCount = 1;
            configInfo.second.viewportInfo.pScissors = nullptr;

            configInfo.second.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            configInfo.second.rasterizationInfo.depthClampEnable = VK_FALSE;
            configInfo.second.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
            configInfo.second.rasterizationInfo.lineWidth = 1.0f;

            configInfo.second.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            configInfo.second.multisampleInfo.alphaToOneEnable = VK_FALSE;

            configInfo.second.colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;

            configInfo.second.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            configInfo.second.colorBlendInfo.logicOpEnable = VK_FALSE;
            configInfo.second.colorBlendInfo.attachmentCount = 1;
            configInfo.second.colorBlendInfo.pAttachments = &configInfo.second.colorBlendAttachment;

            configInfo.second.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            configInfo.second.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
            configInfo.second.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
            configInfo.second.depthStencilInfo.minDepthBounds = 0.0f;
            configInfo.second.depthStencilInfo.maxDepthBounds = 1.0f;
            configInfo.second.depthStencilInfo.stencilTestEnable = VK_FALSE;
            configInfo.second.depthStencilInfo.front = {};
            configInfo.second.depthStencilInfo.back = {};

            configInfo.second.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
            configInfo.second.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            configInfo.second.dynamicStateInfo.pDynamicStates = configInfo.second.dynamicStateEnables.data();
            configInfo.second.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.second.dynamicStateEnables.size());
            configInfo.second.dynamicStateInfo.flags = 0;

            switch (configInfo.first) {
                case PipelineType::SOLID_SPRITE_PIPELINE:
                    configureSolidSpritePipeline(configInfo.second);
                    break;
                case PipelineType::TRANSPARENT_SPRITE_PIPELINE:
                    configureTransparentSpritePipeline(configInfo.second);
                    break;
                case PipelineType::SOLID_OBJECT_PIPELINE:
                    configureSolidObjectPipeline(configInfo.second);
                    break;
                case PipelineType::TRANSPARENT_OBJECT_PIPELINE:
                    configureTransparentObjectPipeline(configInfo.second);
                    break;
                case PipelineType::UI_RENDERING_PIPELINE:
                    configureUIRenderingPipeline(configInfo.second);
                    break;
                case PipelineType::SHADOW_MAPPING_PIPELINE:
                    configureShadowMappingPipeline(configInfo.second);
                    break;
                case PipelineType::SKYBOX_RENDERING_PIPELINE:
                    configureSkyboxRenderingPipeline(configInfo.second);
                    break;
                case PipelineType::PARTICLE_RENDERING_PIPELINE:
                    configureParticleRenderingPipeline(configInfo.second);
                    break;
                default:
                    configurePostProcessingPipeline(configInfo.second);
                    break;
            }
        }
    }

    void GraphicsPipeline::configureSolidSpritePipeline(PipelineConfigInfo& solidSpriteRenderingInfo) {
        std::cout << "Configuring Solid Sprite Pipeline" << std::endl;

        solidSpriteRenderingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        solidSpriteRenderingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        solidSpriteRenderingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        solidSpriteRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        solidSpriteRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        solidSpriteRenderingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        solidSpriteRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        solidSpriteRenderingInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;

        solidSpriteRenderingInfo.colorBlendAttachment.blendEnable = VK_FALSE;

        solidSpriteRenderingInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
        solidSpriteRenderingInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    }

    void GraphicsPipeline::configureTransparentSpritePipeline(PipelineConfigInfo& transparentSpriteRenderingInfo) {
        std::cout << "Configuring Transparent Sprite Pipeline" << std::endl;

        transparentSpriteRenderingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        transparentSpriteRenderingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        transparentSpriteRenderingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        transparentSpriteRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        transparentSpriteRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        transparentSpriteRenderingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        transparentSpriteRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        transparentSpriteRenderingInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;

        transparentSpriteRenderingInfo.colorBlendAttachment.blendEnable = VK_TRUE;
        transparentSpriteRenderingInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        transparentSpriteRenderingInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        transparentSpriteRenderingInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        transparentSpriteRenderingInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        transparentSpriteRenderingInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        transparentSpriteRenderingInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        transparentSpriteRenderingInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
        transparentSpriteRenderingInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    }

    void GraphicsPipeline::configureSolidObjectPipeline(PipelineConfigInfo& solidObjectRenderingInfo) {
        std::cout << "Configuring Solid Object Pipeline" << std::endl;

        solidObjectRenderingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        solidObjectRenderingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        solidObjectRenderingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        solidObjectRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        solidObjectRenderingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        solidObjectRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        solidObjectRenderingInfo.multisampleInfo.sampleShadingEnable = VK_TRUE;
        solidObjectRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
        solidObjectRenderingInfo.multisampleInfo.minSampleShading = 1.0f;
        solidObjectRenderingInfo.multisampleInfo.pSampleMask = nullptr;
        solidObjectRenderingInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;

        solidObjectRenderingInfo.colorBlendAttachment.blendEnable = VK_FALSE;

        solidObjectRenderingInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        solidObjectRenderingInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    }

    void GraphicsPipeline::configureTransparentObjectPipeline(PipelineConfigInfo& transparentObjectRenderingInfo) {
        std::cout << "Configuring Transparent Object Pipeline" << std::endl;

        transparentObjectRenderingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        transparentObjectRenderingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        transparentObjectRenderingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        transparentObjectRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        transparentObjectRenderingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        transparentObjectRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        transparentObjectRenderingInfo.multisampleInfo.sampleShadingEnable = VK_TRUE;
        transparentObjectRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
        transparentObjectRenderingInfo.multisampleInfo.minSampleShading = 1.0f;
        transparentObjectRenderingInfo.multisampleInfo.pSampleMask = nullptr;
        transparentObjectRenderingInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;

        transparentObjectRenderingInfo.colorBlendAttachment.blendEnable = VK_TRUE;
        transparentObjectRenderingInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        transparentObjectRenderingInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        transparentObjectRenderingInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        transparentObjectRenderingInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        transparentObjectRenderingInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        transparentObjectRenderingInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        transparentObjectRenderingInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        transparentObjectRenderingInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    }

    void GraphicsPipeline::configureUIRenderingPipeline(PipelineConfigInfo& UIRenderingInfo) {
        std::cout << "Configuring UI Rendering Pipeline" << std::endl;

        UIRenderingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        UIRenderingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        UIRenderingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        UIRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        UIRenderingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        UIRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        UIRenderingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        UIRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        UIRenderingInfo.multisampleInfo.alphaToCoverageEnable = VK_TRUE;

        UIRenderingInfo.colorBlendAttachment.blendEnable = VK_FALSE;

        UIRenderingInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
        UIRenderingInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    }

    void GraphicsPipeline::configureShadowMappingPipeline(PipelineConfigInfo& shadowMappingInfo) {
        std::cout << "Configuring Shadow Mapping Pipeline" << std::endl;

        shadowMappingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        shadowMappingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        shadowMappingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        shadowMappingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        shadowMappingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        shadowMappingInfo.rasterizationInfo.depthBiasEnable = VK_TRUE;
        shadowMappingInfo.rasterizationInfo.depthBiasConstantFactor = 1.25f;
        shadowMappingInfo.rasterizationInfo.depthBiasSlopeFactor = 1.75f;
        shadowMappingInfo.rasterizationInfo.depthBiasClamp = 0.0f;

        shadowMappingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        shadowMappingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        shadowMappingInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;

        shadowMappingInfo.colorBlendAttachment.blendEnable = VK_FALSE;

        shadowMappingInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        shadowMappingInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    }

    void GraphicsPipeline::configureSkyboxRenderingPipeline(PipelineConfigInfo& skyboxRenderingInfo) {
        std::cout << "Configuring Skybox Rendering Pipeline" << std::endl;

        skyboxRenderingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        skyboxRenderingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        skyboxRenderingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        skyboxRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
        skyboxRenderingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        skyboxRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        skyboxRenderingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        skyboxRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        skyboxRenderingInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;

        skyboxRenderingInfo.colorBlendAttachment.blendEnable = VK_FALSE;

        skyboxRenderingInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        skyboxRenderingInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    }

    void GraphicsPipeline::configureParticleRenderingPipeline(PipelineConfigInfo& particleRenderingInfo) {
        std::cout << "Configuring Particle Rendering Pipeline" << std::endl;

        particleRenderingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        particleRenderingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        particleRenderingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_POINT;
        particleRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        particleRenderingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        particleRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        particleRenderingInfo.multisampleInfo.sampleShadingEnable = VK_TRUE;
        particleRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
        particleRenderingInfo.multisampleInfo.minSampleShading = 0.5f;
        particleRenderingInfo.multisampleInfo.pSampleMask = nullptr;
        particleRenderingInfo.multisampleInfo.alphaToCoverageEnable = VK_TRUE;

        particleRenderingInfo.colorBlendAttachment.blendEnable = VK_TRUE;
        particleRenderingInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        particleRenderingInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        particleRenderingInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        particleRenderingInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        particleRenderingInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        particleRenderingInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        particleRenderingInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        particleRenderingInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    }

    void GraphicsPipeline::configurePostProcessingPipeline(PipelineConfigInfo& postProcessingInfo) {
        std::cout << "Configuring Post Processing Pipeline" << std::endl;

        postProcessingInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        postProcessingInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        postProcessingInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        postProcessingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        postProcessingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        postProcessingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        postProcessingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        postProcessingInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;

        postProcessingInfo.colorBlendAttachment.blendEnable = VK_TRUE;
        postProcessingInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        postProcessingInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        postProcessingInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        postProcessingInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        postProcessingInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        postProcessingInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        postProcessingInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
        postProcessingInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    }

    // Eventually will need to add support for other file types, not just shaders
    // We will be loading textures, blender files, may have computational shaders for physics, etc...

    void GraphicsPipeline::createSolidSpritePipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        // Pipeline layout defined in actual application due to the customization of aspects like the PushConstantRange.
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        // Might create the renderPass either in the swapChain file or the resourceManager File
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createTransparentSpritePipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createSolidObjectPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createTransparentObjectPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createUIRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createShadowMappingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createSkyboxRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createParticleRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    void GraphicsPipeline::createPostProcessingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);
    }

    std::vector<VkPipelineShaderStageCreateInfo> GraphicsPipeline::createShaderStages(const std::string& vertFilepath, const std::string& fragFilepath) {
        std::vector<char> vertexCode = ResourceManager::readFile(vertFilepath);
        std::vector<char> fragmentCode = ResourceManager::readFile(fragFilepath);

        std::cout << "Vertex File Size: " << vertexCode.size() << std::endl;
        std::cout << "Fragment File Size: " << fragmentCode.size() << std::endl;

        createShaderModule(vertexCode, &vertShaderModule);
        createShaderModule(fragmentCode, &fragShaderModule);

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        return shaderStages;
    }

    void GraphicsPipeline::createShaderModule(const std::vector<char>& shaderBinaryCode, VkShaderModule* shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderBinaryCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBinaryCode.data());

        if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the shader module!");
        }
    }
}