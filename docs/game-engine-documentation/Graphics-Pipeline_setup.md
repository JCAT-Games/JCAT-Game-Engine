# Graphics Pipeline Setup

To help create a Vulkan game engine, this [youtube playlist](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR) was used.

This file contains the instructions for setting up the graphics pipeline for Vulkan.

## Step 0: Read the shader files:

The first step to the grpahics pipeline is to read the binary data of the compiled .vert and .frag shader files.

This can be done with the following code:

```cpp
std::vector<char> LvePipeline::readFile(const std::string& filepath) {
    //Ensures that the binary of the file is read
	std::ifstream file{ filepath, std::ios::ate | std::ios::binary }; 

    // If the file is unable to be opened
	if (!file.is_open()) {
		throw std::runtime_error("failed to open file: " + filepath);
	}

    // Creates a vector of charecters the size of the current file size
	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

    //Reads the binary data to the "buffer" vector
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}
```

## Step 1: Create Shader Modules:

A Vulkan shader module is a reosurce that represents a compiled shader in the Vulkan API. This will then be used to incorperate the sahder into the rendering pipeline.

The shader module can be created with the following function:

```cpp
void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
    // Use the VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO to create a shader module
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module");
	}
}
```

* Pass both the .vert and the .frag shader files into this function

## Step 2: Create the shader stages:

The shader stages define the shaders that will be used in the graphics pipeline. 
Here, we must specify the vertex and fragment shader stages for the pipeline.

To configure the shader stages, we will use the following code:

```cpp
VkPipelineShaderStageCreateInfo shaderStages[2];
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
```

We create two pipeline shader stage structures. One for the vertex shaders and one for the fragment shaders.

## Step 3: Define vertex input size and the viewport:

The vertex input is responsible for handling the vertex data that is passed into the GPU for rendering. 
The viewport state controls how the rendering results are mapped to the screen. 

We must define both of these for the graphics pipeline:

```cpp
VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
vertexInputInfo.vertexAttributeDescriptionCount = 0;
vertexInputInfo.vertexBindingDescriptionCount = 0;
vertexInputInfo.pVertexAttributeDescriptions = nullptr;
vertexInputInfo.pVertexBindingDescriptions = nullptr;

VkPipelineViewportStateCreateInfo viewportInfo{};
viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
viewportInfo.viewportCount = 1;
viewportInfo.pViewports = &configInfo.viewport;
viewportInfo.scissorCount = 1;
viewportInfo.pScissors = &configInfo.scissor;
```

We will be using a Vulkan structure for each of these.

## Step 4: Configure the graphics pipeline:

Now, we must configure the graphics pipeline. To configure the graphics pipeline, we use the following code:

```cpp
VkGraphicsPipelineCreateInfo pipelineInfo{};
pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
pipelineInfo.stageCount = 2;
pipelineInfo.pStages = shaderStages;
pipelineInfo.pVertexInputState = &vertexInputInfo;
pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
pipelineInfo.pViewportState = &viewportInfo;
pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
pipelineInfo.pDynamicState = nullptr;

pipelineInfo.layout = configInfo.pipelineLayout;
pipelineInfo.renderPass = configInfo.renderPass;
pipelineInfo.subpass = configInfo.subpass;

pipelineInfo.basePipelineIndex = -1;
pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

if (vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
	throw std::runtime_error("failed to create graphics pipeline");
}
```

After configuration, the graphics pipeline is created.

The Vulkan graphics pipeline has several stages to it, which include the following:

### Stage 1: Initializaion of VkGraphicsPipelineInfo:

This structure holds all of the information ecessary to create the Vulkan graphics pipeline.
This structure is of type: ```VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO```

The whole goal of the graphics pipeline is to transform the raw input data which was read earlier to the final rendered image which is dispayed on the screen.

### Stage 2: Declaring Shader Stages:

We then configure ths shader stages of the graphics pipeline. We do this with the shader stages variable we configured earlier. 

For this game engine, we will be using 2 shader stages, one for the vertex shaders and one fo rthe fragment shaders.

### Stage 3: Vertex Input State:

The next stage is the vert input state which defines how vertex data is provided to the pipeline. 

For thie, we will be pointing to the vertexINputInfo variable we configured earlier.

### Stage 4: Input Assembly State:

This stage defines the type of primitives to assemble (like points, lines, or triangles) for rendering the input data.

In this case, we will be using triangles.

### Stage 5: Viewport State:

Define the viewport and scissor rectangle configurations. This controls the region of the framebuffer that will be rendered into. 

For this game engine, we would want both the viewport and the scissor rectangle to be the size and the location of our window.

### Stage 6: Rasterization State:

This stages defines how the primative graphic structures like points, lines, and tringles, are comverted into fragmets that can be rendered onto the screen.

### Stage 7: Multisample State:

The multisampling stage determines how aspects of anti-aliasing is handled. 

Multisampling allows for smoother edges by sampling a pixel multiple times ad averaging the results.

### Stage 8: Color Blend State:

This stage defines how color belding is handled during the graphics pipeline for the fragment shader. 

This stage is basically used to blend nearby colors together.

### Stage 9: Depth-Stencil State:

This stage defines how depth testig and stencil testing is preformed.

Depth testing controls whether fragments are rendered base don their depth.

Stencil testing controls more advamced features like shadow volumns.

### Stage 10: Dynamic State:

This stage allows certain parts of the pipeline to be changed without needing to recreate the whole pipeline.

### Stage 11: Pipeline Layout:

This stage describes the resources that the pipeline will use.

### Stage 12: Render Pass and Subpass:

Specifies which render pass the pipeline will use, which describes the framebuffer attachments, and how rendering is done.

### Stage 13: Base Pipeline:

Sepcifies whether a base pipeline will be used.

In this case, no base pipeline is used.
