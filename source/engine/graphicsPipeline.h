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
        //PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        //PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo; ///< State for the viewport and scissor rectangle settings.
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo; ///< Configuration for primitive assembly topology.
        VkPipelineRasterizationStateCreateInfo rasterizationInfo; ///< Settings for rasterizing primitives, including culling mode.
        VkPipelineMultisampleStateCreateInfo multisampleInfo; ///< Multisampling settings to enhance image quality.
        VkPipelineColorBlendAttachmentState colorBlendAttachment; ///< Vector of blending configurations for multiple color attachments.
        VkPipelineColorBlendStateCreateInfo colorBlendInfo; ///< Color blending settings for framebuffer operations.
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo; ///< Depth and stencil testing configurations.
        std::vector<VkDynamicState> dynamicStateEnables; ///< Vector of dynamic state enums that can be modified at draw time.
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr; ///< Layout defining descriptor sets and push constants.
        VkRenderPass renderPass = nullptr; ///< Render pass managing framebuffer and attachment usage.
        uint32_t subpass = 0; ///< Index of the subpass linked to this pipeline.
    };

    class GraphicsPipeline {
        public:
            enum class PipelineType {
                SOLID_SPRITE_PIPELINE,
                TRANSPARENT_SPRITE_PIPELINE,
                SOLID_OBJECT_PIPELINE,
                TRANSPARENT_OBJECT_PIPELINE,
                UI_RENDERING_PIPELINE,
                SHADOW_MAPPING_PIPELINE,
                SKYBOX_RENDERING_PIPELINE,
                PARTICLE_RENDERING_PIPELINE,
                POST_PROCESSING_PIPELINE
            };

            GraphicsPipeline(DeviceSetup &physicalDevice, 
                             ResourceManager &resourceManager, 
                             const std::string& vertFilepath, 
                             const std::string& fragfilepath, 
                             const std::unordered_map<std::string, PipelineConfigInfo>& configInfos);
            ~GraphicsPipeline();

            GraphicsPipeline(const GraphicsPipeline&) = delete;
            void operator=(const GraphicsPipeline&) = delete;

            void bindPipeline(VkCommandBuffer commandBuffer, PipelineType type);
            static void configurePipelines(std::unordered_map<PipelineType, PipelineConfigInfo>& configInfos);

            static void configureSolidSpritePipeline(PipelineConfigInfo& solidSpriteRenderingInfo);
            static void configureTransparentSpritePipeline(PipelineConfigInfo& transparentSpriteRenderingInfo);
            static void configureSolidObjectPipeline(PipelineConfigInfo& solidObjectRenderingInfo);
            static void configureTransparentObjectPipeline(PipelineConfigInfo& transparentObjectRenderingInfo);
            static void configureUIRenderingPipeline(PipelineConfigInfo& UIRenderingInfo);
            static void configureShadowMappingPipeline(PipelineConfigInfo& shadowMappingInfo);
            static void configureSkyboxRenderingPipeline(PipelineConfigInfo& skyboxRenderingInfo);
            static void configureParticleRenderingPipeline(PipelineConfigInfo& particleRenderingInfo);
            static void configurePostProcessingPipeline(PipelineConfigInfo& postProcessingInfo);
        private:
            void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragfilepath, const std::unordered_map<std::string, PipelineConfigInfo>& configInfos);
            void createShaderModule(const std::vector<char>& shaderBinaryCode, VkShaderModule* shaderModule);

            DeviceSetup &device;
            ResourceManager &resources;
            std::unordered_map<PipelineType, VkPipeline> graphicsPipelines;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
};

#endif