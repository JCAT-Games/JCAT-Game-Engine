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
        configInfos.insert({"ObjectRendering", PipelineConfigInfo{}});
        configInfos.insert({});

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

            configInfo.second.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            configInfo.second.multisampleInfo.sampleShadingEnable = VK_FALSE;
            configInfo.second.multisampleInfo.alphaToOneEnable = VK_FALSE;

            configInfo.second.colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;

            configInfo.second.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

            configInfo.second.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

            configInfo.second.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
            configInfo.second.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
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