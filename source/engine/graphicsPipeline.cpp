#include "./engine/graphicsPipeline.h"

namespace JCAT {
    GraphicsPipeline::GraphicsPipeline(DeviceSetup &physicalDevice, ResourceManager &resourceManager, const std::string& vertFilepath, const std::string& fragfilepath, const std::unordered_map<std::string, PipelineConfigInfo>& configInfos) : device{ physicalDevice }, resources{ resourceManager } {
        createGraphicsPipeline(vertFilepath, fragfilepath, configInfos);
    }

    GraphicsPipeline::~GraphicsPipeline() {
        vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);

        for (std::pair<const std::string, VkPipeline>& graphicsPipeline : graphicsPipelines) {
            vkDestroyPipeline(device.device(), graphicsPipeline.second, nullptr);
        }
    }

    void GraphicsPipeline::bindPipeline(VkCommandBuffer commandBuffer, std::string type) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[type]);
    }

    void GraphicsPipeline::configurePipeline(std::unordered_map<std::string, PipelineConfigInfo>& configInfos) {
        configInfos.insert({"SpriteRendering", PipelineConfigInfo{}});
        configInfos.insert({"ObjectRendering", PipelineConfigInfo{}});
        configInfos.insert({"UIRendering", PipelineConfigInfo{}});
        configInfos.insert({"ShadowMapping", PipelineConfigInfo{}});
        configInfos.insert({"SkyboxRendering", PipelineConfigInfo{}});
        configInfos.insert({"ParticleRendering", PipelineConfigInfo{}});
        configInfos.insert({"PostProcessing", PipelineConfigInfo{}});

        for (std::pair<const std::string, PipelineConfigInfo>& configInfo : configInfos) {
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

            configInfo.second.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

            configInfo.second.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
            configInfo.second.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            configInfo.second.dynamicStateInfo.pDynamicStates = configInfo.second.dynamicStateEnables.data();
            configInfo.second.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.second.dynamicStateEnables.size());
            configInfo.second.dynamicStateInfo.flags = 0;

            if (configInfo.first == "SpriteRendering") {
                configInfo.second.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                configInfo.second.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

                configInfo.second.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
                configInfo.second.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
                configInfo.second.rasterizationInfo.depthBiasEnable = VK_FALSE;

                configInfo.second.multisampleInfo.sampleShadingEnable = VK_FALSE;
                configInfo.second.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
                configInfo.second.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
            }
            else if (configInfo.first == "ObjectRendering") {
                configInfo.second.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                configInfo.second.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

                configInfo.second.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
                configInfo.second.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
                configInfo.second.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
                configInfo.second.rasterizationInfo.depthBiasEnable = VK_FALSE;

                configInfo.second.multisampleInfo.sampleShadingEnable = VK_TRUE;
                configInfo.second.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
                configInfo.second.multisampleInfo.minSampleShading = 1.0f;
                configInfo.second.multisampleInfo.pSampleMask = nullptr;
                configInfo.second.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
            }
            else if (configInfo.first == "UIRendering") {
                configInfo.second.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                configInfo.second.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

                configInfo.second.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
                configInfo.second.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
                configInfo.second.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
                configInfo.second.rasterizationInfo.depthBiasEnable = VK_FALSE;

                configInfo.second.multisampleInfo.sampleShadingEnable = VK_FALSE;
                configInfo.second.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
                configInfo.second.multisampleInfo.alphaToCoverageEnable = VK_TRUE;

            }
            else if (configInfo.first == "ShadowMapping") {
                configInfo.second.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                configInfo.second.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

                configInfo.second.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
                configInfo.second.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
                configInfo.second.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
                configInfo.second.rasterizationInfo.depthBiasEnable = VK_TRUE;
                configInfo.second.rasterizationInfo.depthBiasConstantFactor = 1.25f;
                configInfo.second.rasterizationInfo.depthBiasSlopeFactor = 1.75f;
                configInfo.second.rasterizationInfo.depthBiasClamp = 0.0f;

                configInfo.second.multisampleInfo.sampleShadingEnable = VK_FALSE;
                configInfo.second.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
                configInfo.second.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
            }
            else if (configInfo.first == "SkyboxRendering") {
                configInfo.second.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                configInfo.second.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

                configInfo.second.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
                configInfo.second.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
                configInfo.second.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                configInfo.second.rasterizationInfo.depthBiasEnable = VK_FALSE;

                configInfo.second.multisampleInfo.sampleShadingEnable = VK_FALSE;
                configInfo.second.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
                configInfo.second.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
            }
            else if (configInfo.first == "ParticleRendering") {
                configInfo.second.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                configInfo.second.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

                configInfo.second.rasterizationInfo.polygonMode = VK_POLYGON_MODE_POINT;
                configInfo.second.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
                configInfo.second.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
                configInfo.second.rasterizationInfo.depthBiasEnable = VK_FALSE;

                configInfo.second.multisampleInfo.sampleShadingEnable = VK_TRUE;
                configInfo.second.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
                configInfo.second.multisampleInfo.minSampleShading = 0.5f;
                configInfo.second.multisampleInfo.pSampleMask = nullptr;
                configInfo.second.multisampleInfo.alphaToCoverageEnable = VK_TRUE;
            }
            else {
                configInfo.second.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                configInfo.second.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

                configInfo.second.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
                configInfo.second.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
                configInfo.second.rasterizationInfo.depthBiasEnable = VK_FALSE;

                configInfo.second.multisampleInfo.sampleShadingEnable = VK_FALSE;
                configInfo.second.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
                configInfo.second.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
            }
        }
    }

    void GraphicsPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragfilepath, const std::unordered_map<std::string, PipelineConfigInfo>& configInfos) {

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