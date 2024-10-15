#include "./engine/graphicsPipeline.h"

namespace JCAT {
    GraphicsPipeline::GraphicsPipeline(DeviceSetup &phsyicalDevice, const std::string& vertFilepath, const std::string& fragfilepath, const PipelineConfigInfo& configInfo) : device{ phsyicalDevice } {
        
    }

    GraphicsPipeline::~GraphicsPipeline() {

    }

    void GraphicsPipeline::bindPipeline(VkCommandBuffer commandBuffer) {

    }

    void GraphicsPipeline::configurePipeline(PipelineConfigInfo& configInfo) {

    }
}