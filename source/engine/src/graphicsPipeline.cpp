#include <cassert>

#include "./engine/graphicsPipeline.h"
#include "./engine/2d/model2d.h"
#include "./engine/3d/model3d.h"

namespace JCAT {
    /// @brief Constructs a GraphicsPipeline object.
    /// @param physicalDevice Reference to the physical device setup.
    /// @param resourceManager Reference to the resource manager.
    /// @param vertFilepath Path to the vertex shader file.
    /// @param fragFilepath Path to the fragment shader file.
    GraphicsPipeline::GraphicsPipeline(DeviceSetup &physicalDevice, ResourceManager &resourceManager, const std::string& vertFilepath, const std::string& fragfilepath) : device{ physicalDevice }, resources{ resourceManager } {
        // Initialize graphics pipelines with default VK_NULL_HANDLE values
        graphicsPipelines = {
            {PipelineType::SOLID_SPRITE_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::TRANSPARENT_SPRITE_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::SOLID_OBJECT_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::TRANSPARENT_OBJECT_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::UI_RENDERING_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::SHADOW_MAPPING_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::SKYBOX_RENDERING_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::PARTICLE_RENDERING_PIPELINE, VK_NULL_HANDLE},
            {PipelineType::POST_PROCESSING_PIPELINE, VK_NULL_HANDLE}
        };
    }

    /// @brief Destructor that cleans up Vulkan shader modules and pipelines.
    GraphicsPipeline::~GraphicsPipeline() {
        vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);

        for (std::pair<const PipelineType, VkPipeline>& graphicsPipeline : graphicsPipelines) {
            vkDestroyPipeline(device.device(), graphicsPipeline.second, nullptr);
        }
    }

    /// @brief Retrieves the Vulkan pipeline associated with a given type.
    /// @param type The type of pipeline to retrieve.
    /// @return Reference to the Vulkan pipeline.
    /// @throws std::runtime_error if the pipeline type is not found.
    VkPipeline& GraphicsPipeline::getPipeline(PipelineType type) {
        std::unordered_map<PipelineType, VkPipeline>::iterator it = graphicsPipelines.find(type);
        
        if (it != graphicsPipelines.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Pipeline of this type not found!");
        }
    }

    /// @brief Binds the specified pipeline to the given command buffer.
    /// @param commandBuffer The command buffer to bind the pipeline to.
    /// @param type The type of pipeline to bind.
    void GraphicsPipeline::bindPipeline(VkCommandBuffer commandBuffer, PipelineType type) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[type]);
    }

    /// @brief Configures Vulkan pipeline settings for various rendering tasks.
    /// @param configInfos A map to store pipeline configurations.
    void GraphicsPipeline::configurePipelines(std::unordered_map<PipelineType, PipelineConfigInfo>& configInfos) {
        std::cout << "Configuring Pipelines" << std::endl;

        // Initialize configurations for different pipeline types
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

            // Configure specific pipeline settings
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
        solidSpriteRenderingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        solidSpriteRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        solidSpriteRenderingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        solidSpriteRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        solidSpriteRenderingInfo.multisampleInfo.minSampleShading = 1.0f;
        solidSpriteRenderingInfo.multisampleInfo.pSampleMask = nullptr;
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
        solidObjectRenderingInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        solidObjectRenderingInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        solidObjectRenderingInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

        solidObjectRenderingInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        solidObjectRenderingInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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

        UIRenderingInfo.colorBlendAttachment.blendEnable = VK_TRUE;
        UIRenderingInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        UIRenderingInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        UIRenderingInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        UIRenderingInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        UIRenderingInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

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

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions2D();

        createPipeline(getPipeline(PipelineType::SOLID_SPRITE_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createTransparentSpritePipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions2D();

        createPipeline(getPipeline(PipelineType::TRANSPARENT_SPRITE_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createSolidObjectPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidObjectRenderingInfo) {
        assert(solidObjectRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidObjectRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions3D();

        createPipeline(getPipeline(PipelineType::SOLID_OBJECT_PIPELINE), solidObjectRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createTransparentObjectPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions3D();

        createPipeline(getPipeline(PipelineType::TRANSPARENT_OBJECT_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createUIRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions2D();

        createPipeline(getPipeline(PipelineType::UI_RENDERING_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createShadowMappingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions2D();

        createPipeline(getPipeline(PipelineType::SHADOW_MAPPING_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createSkyboxRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions3D();

        createPipeline(getPipeline(PipelineType::SKYBOX_RENDERING_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createParticleRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions2D();

        createPipeline(getPipeline(PipelineType::PARTICLE_RENDERING_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createPostProcessingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo) {
        assert(solidSpriteRenderingInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo!");
        assert(solidSpriteRenderingInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo!");
    
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = createShaderStages(vertFilepath, fragfilepath);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = getDescriptions2D();

        createPipeline(getPipeline(PipelineType::POST_PROCESSING_PIPELINE), solidSpriteRenderingInfo, shaderStages, vertexInputInfo);
    }

    void GraphicsPipeline::createPipeline(VkPipeline& graphicsPipeline, PipelineConfigInfo& configInfo, std::vector<VkPipelineShaderStageCreateInfo>& shaderStages, VkPipelineVertexInputStateCreateInfo& vertexInputInfo) {
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

        pipelineInfo.layout = configInfo.pipelineLayout;
        pipelineInfo.renderPass = configInfo.renderPass;
        pipelineInfo.subpass = configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
	        throw std::runtime_error("Failed to create this graphics pipeline!");
        }
    }

    VkPipelineVertexInputStateCreateInfo GraphicsPipeline::getDescriptions2D() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions = JCATModel2D::Vertex2D::getBindingDescriptions();
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions = JCATModel2D::Vertex2D::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        return vertexInputInfo;
    }

    VkPipelineVertexInputStateCreateInfo GraphicsPipeline::getDescriptions3D() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions = JCATModel3D::Vertex3D::getBindingDescriptions();
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions = JCATModel3D::Vertex3D::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        return vertexInputInfo;
    }

    std::vector<VkPipelineShaderStageCreateInfo> GraphicsPipeline::createShaderStages(const std::string& vertFilepath, const std::string& fragFilepath) {
        std::vector<char> vertexCode = ResourceManager::readFile(vertFilepath);
        std::vector<char> fragmentCode = ResourceManager::readFile(fragFilepath);

        std::cout << "Vertex File Size: " << vertexCode.size() << std::endl;
        std::cout << "Fragment File Size: " << fragmentCode.size() << std::endl;

        createShaderModule(vertexCode, &vertShaderModule);
        createShaderModule(fragmentCode, &fragShaderModule);

        std::cout << "Created Shader Modules!" << std::endl;

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        VkPipelineShaderStageCreateInfo vertexStage{};
        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStage.module = vertShaderModule;
        vertexStage.pName = "main";
        vertexStage.flags = 0;
        vertexStage.pNext = nullptr;
        vertexStage.pSpecializationInfo = nullptr;
        shaderStages.push_back(vertexStage);

        VkPipelineShaderStageCreateInfo fragmentStage{};
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStage.module = fragShaderModule;
        fragmentStage.pName = "main";
        fragmentStage.flags = 0;
        fragmentStage.pNext = nullptr;
        fragmentStage.pSpecializationInfo = nullptr;
        shaderStages.push_back(fragmentStage);

        std::cout << "Created Shader Stages!" << std::endl;

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
};