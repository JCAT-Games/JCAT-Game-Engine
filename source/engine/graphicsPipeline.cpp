#include "./engine/graphicsPipeline.h"

namespace JCAT {
    GraphicsPipeline::GraphicsPipeline(DeviceSetup &physicalDevice, ResourceManager &resourceManager, const std::string& vertFilepath, const std::string& fragfilepath, const std::vector<PipelineConfigInfo>& configInfos) : device{ physicalDevice }, resources{ resourceManager } {
        createGraphicsPipeline(vertFilepath, fragfilepath, configInfos);
    }

    GraphicsPipeline::~GraphicsPipeline() {

    }

    void GraphicsPipeline::bindPipeline(VkCommandBuffer commandBuffer, int type) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[type]);
    }

    void GraphicsPipeline::configurePipeline(std::vector<PipelineConfigInfo>& configInfos) {
        for (int i = 0; i < configInfos.size(); i++) {

        }
    }

    void GraphicsPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragfilepath, const std::vector<PipelineConfigInfo>& configInfos) {

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