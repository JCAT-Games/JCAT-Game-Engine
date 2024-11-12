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
     * - **dynamicStateInfo**: Specifies the dynamic states (settings modified between draw calls) of the pipeline.\n
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
        VkPipelineViewportStateCreateInfo viewportInfo; ///< State for the viewport and scissor rectangle settings.
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo; ///< Configuration for primitive assembly topology.
        VkPipelineRasterizationStateCreateInfo rasterizationInfo; ///< Settings for rasterizing primitives, including culling mode.
        VkPipelineMultisampleStateCreateInfo multisampleInfo; ///< Multisampling settings to enhance image quality.
        VkPipelineColorBlendAttachmentState colorBlendAttachment; ///< Vector of blending configurations for multiple color attachments.
        VkPipelineColorBlendStateCreateInfo colorBlendInfo; ///< Color blending settings for framebuffer operations.
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo; ///< Depth and stencil testing configurations.
        std::vector<VkDynamicState> dynamicStateEnables; ///< Vector of dynamic state enums that can be modified at draw time.
        VkPipelineDynamicStateCreateInfo dynamicStateInfo; ///< Specifies the dynamic states (settings modified between draw calls) of the pipeline.
        VkPipelineLayout pipelineLayout = nullptr; ///< Layout defining descriptor sets and push constants.
        VkRenderPass renderPass = nullptr; ///< Render pass managing framebuffer and attachment usage.
        uint32_t subpass = 0; ///< Index of the subpass linked to this pipeline.
    };

    /**
     * @class GraphicsPipeline
     * @brief Represents a Vulkan graphics pipeline class for the JCAT Game Engine.
     *
     * This class encapsulates the setup and management of all the different graphics pipelines
     * used in the JCAT Game Engine. This includes pipeline for both 2D and 3D rendering.
     * This class also encapsulates many of the different graphics settings that could be
     * used in a game.
     *
     * @details This class is responsible for configuring all the different types of pipelines.
     * It is also responsible for creating all the necessary pipelines and managing their individual settings.
     * It also ensures ease of access to all the different pipelines through enums.
     */
    class GraphicsPipeline {
        public:

            /**
             * @enum PipelineType
             * 
             * @brief Defines calls to different pipeline types used for rendering in the graphics engine.
             *
             * Each pipeline type corresponds to a different rendering setup, allowing
             * for optimized handling of various rendering tasks in both 2D and 3D graphics.
             *
             * - SOLID_SPRITE_PIPELINE: Renders opaque 2D sprites without transparency.
             * - TRANSPARENT_SPRITE_PIPELINE: Renders 2D sprites with transparency support.
             * - SOLID_OBJECT_PIPELINE: Renders solid 3D objects without transparency.
             * - TRANSPARENT_OBJECT_PIPELINE: Renders 3D objects with transparency enabled.
             * - UI_RENDERING_PIPELINE: Used specifically for rendering 2D UI elements.
             * - SHADOW_MAPPING_PIPELINE: Configured for shadow map generation.
             * - SKYBOX_RENDERING_PIPELINE: Renders skyboxes for background scenery.
             * - PARTICLE_RENDERING_PIPELINE: Optimized for rendering particle systems.
             * - POST_PROCESSING_PIPELINE: Used for applying post-processing effects (e.g., bloom, tone mapping).
             */
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

            /** ### Constructor & Destructor ### */

            /**
             * @brief Constructs a GraphicsPipeline instance for Vulkan graphics pipeline management.
             *
             * @param physicalDevice Reference to the device object that stores information about the users physical and logical device.
             * @param resourceManager Stores information about specific resources, such as buffers and resource reading functions.
             * @param vertFilepath Path to the compiled vertex shader file that will be loaded into the graphics pipeline.
             * @param fragfilepath Path to the compiled fragment shader file that will be loaded into the graphics pipeline. 
             */
            GraphicsPipeline(DeviceSetup &physicalDevice, 
                             ResourceManager &resourceManager, 
                             const std::string& vertFilepath, 
                             const std::string& fragfilepath
            );

            /**
             * @brief Destroys the GraphicsPipeline instance and releases associated resources from memory.
             */
            ~GraphicsPipeline();

            /** ### Delete Copy and Assignment operators ### */
            GraphicsPipeline(const GraphicsPipeline&) = delete;
            void operator=(const GraphicsPipeline&) = delete;

            /* ### Accessors ### */

            /**
             * @brief Returns a pointer to the pipeline of the specified type if it exists in the graphicsPipelines unordered map.
             * 
             * @param type - Enum specifying the type of pipeline to return.
             *
             * @return VkPipeline& - A pointer to the graphics pipeline of the specified type.
             */
            VkPipeline& getPipeline(PipelineType type);

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

            void createSolidSpritePipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createTransparentSpritePipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createSolidObjectPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createTransparentObjectPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createUIRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createShadowMappingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createSkyboxRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createParticleRenderingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
            void createPostProcessingPipeline(const std::string& vertFilepath, const std::string& fragfilepath, PipelineConfigInfo& solidSpriteRenderingInfo);
        
        private:
            void createPipeline(VkPipeline& graphicsPipeline, 
                                PipelineConfigInfo& configInfo, 
                                std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
                                VkPipelineVertexInputStateCreateInfo& vertexInputInfo);

            VkPipelineVertexInputStateCreateInfo getDescriptions2D();
            VkPipelineVertexInputStateCreateInfo getDescriptions3D();

            std::vector<VkPipelineShaderStageCreateInfo> createShaderStages(const std::string& vertFilepath, const std::string& fragFilepath);
            void createShaderModule(const std::vector<char>& shaderBinaryCode, VkShaderModule* shaderModule);

            DeviceSetup &device;
            ResourceManager &resources;
            std::unordered_map<PipelineType, VkPipeline> graphicsPipelines;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
};

#endif