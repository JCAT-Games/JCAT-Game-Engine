#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace JCAT {

    /**
     * @brief Configuration settings for the Vulkan graphics pipeline.
     * 
     * The `PipelineConfigInfo` struct holds all necessary consistent parameters to create and
     * configure a Vulkan graphics pipeline, enabling customizable rendering behaviors.
     * 
     * ## Members:
     * 
     * - **viewportState**: State for the viewport and scissor rectangle settings.\n
     * 
     * - **inputAssemblyInfo**: Configuration for primitive assembly topology.\n
     * 
     * - **rasterizationInfo**: Settings for rasterizing primitives, including culling mode.\n
     * 
     * - **multisampleInfo**: Multisampling settings to enhance image quality.\n
     * 
     * - **colorBlendAttachments**: Vector of blending configurations for multiple color attachments.\n
     * 
     * - **colorBlendInfo**: Color blending settings for framebuffer operations.\n
     * 
     * - **depthStencilInfo**: Depth and stencil testing configurations.\n
     * 
     * - **dynamicStateEnables**: Vector of dynamic state enums that can be modified at draw time.\n
     * 
     * - **pipelineLayout**: Layout defining descriptor sets and push constants.\n
     * 
     * - **renderPass**: Render pass managing framebuffer and attachment usage.\n
     * 
     * - **subpass**: Index of the subpass linked to this pipeline.\n
     * 
     * 
     * @note This struct is intended to be a static configuration, and should be
     *       defined once and reused to maintain consistent pipeline settings. 
     *       It has a deleted copy constructor and assignment operator to prevent 
     *       unintended copying.
     */
    struct PipelineConfigInfo {
        /** ### Delete Copy and Assignment operators ### */
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportState; ///< State for the viewport and scissor rectangle settings.
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo; ///< Configuration for primitive assembly topology.
        VkPipelineRasterizationStateCreateInfo rasterizationInfo; ///< Settings for rasterizing primitives, including culling mode.
        VkPipelineMultisampleStateCreateInfo multisampleInfo; ///< Multisampling settings to enhance image quality.
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments; ///< Vector of blending configurations for multiple color attachments.
        VkPipelineColorBlendStateCreateInfo colorBlendInfo; ///< Color blending settings for framebuffer operations.
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo; ///< Depth and stencil testing configurations.
        std::vector<VkDynamicState> dynamicStateEnables; ///< Vector of dynamic state enums that can be modified at draw time.
        VkPipelineLayout pipelineLayout = nullptr; ///< Layout defining descriptor sets and push constants.
        VkRenderPass renderPass = nullptr; ///< Render pass managing framebuffer and attachment usage.
        uint32_t subpass = 0; ///< Index of the subpass linked to this pipeline.
    };

    class GraphicsPipeline {
        public:
            GraphicsPipeline(DeviceSetup &physicalDevice, 
                             ResourceManager &resourceManager, 
                             const std::string& vertFilepath, 
                             const std::string& fragfilepath, 
                             const std::vector<PipelineConfigInfo>& configInfos);
            ~GraphicsPipeline();

            GraphicsPipeline(const GraphicsPipeline&) = delete;
            void operator=(const GraphicsPipeline&) = delete;

            void bindPipeline(VkCommandBuffer commandBuffer, int type);
            static void configurePipeline(std::vector<PipelineConfigInfo>& configInfos);
        private:
            void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragfilepath, const std::vector<PipelineConfigInfo>& configInfos);
            void createShaderModule(const std::vector<char>& shaderBinaryCode, VkShaderModule* shaderModule);

            DeviceSetup &device;
            ResourceManager &resources;
            // Will change to a map
            std::vector<VkPipeline> graphicsPipelines;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}

#endif