#include <iostream>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#include "./engine/resourceManager.h"

namespace JCAT {
    ResourceManager::ResourceManager(DeviceSetup& device) : device_{ device } {}

    std::vector<char> ResourceManager::readFile(const std::string& filepath) {
        std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file at: " + filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    ImageData ResourceManager::loadImage(const std::string& filepath, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically); // Flip image if needed

        int width, height, channels;
        unsigned char* pixels = stbi_load(
            filepath.c_str(), 
            &width, 
            &height, 
            &channels, 
            STBI_rgb_alpha // Force 4 channels (RGBA)
        );

        if (!pixels) {
            throw std::runtime_error("Failed to load image: " + filepath);
        }

        // Copy pixel data into a vector for automatic memory management
        std::vector<unsigned char> pixelData(width * height * 4);
        std::memcpy(pixelData.data(), pixels, width * height * 4);
        stbi_image_free(pixels); // Free original data

        return {width, height, 4, pixelData}; // Channels forced to 4 by STBI_rgb_alpha
    }

    void ResourceManager::createBuffer(VkDeviceSize size,
                                        VkBufferUsageFlags usage,
                                        VkMemoryPropertyFlags properties,
                                        VkBuffer& buffer,
                                        VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device_.device(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }

        VkMemoryRequirements bufferMemRequirements;
        vkGetBufferMemoryRequirements(device_.device(), buffer, &bufferMemRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = bufferMemRequirements.size;
        allocInfo.memoryTypeIndex = device_.findMemoryType(bufferMemRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device_.device(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device_.device(), buffer, bufferMemory, 0);
    }

    VkCommandBuffer ResourceManager::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device_.getCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device_.device(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void ResourceManager::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(device_.graphicsQueue());

        vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), 1, &commandBuffer);
    }

    void ResourceManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;

        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }

    void ResourceManager::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(commandBuffer);
    }

    void ResourceManager::createImageWithInfo(const VkImageCreateInfo& imageInfo,
                                                VkMemoryPropertyFlags properties,
                                                VkImage& image,
                                                VkDeviceMemory& imageMemory) {
        if (vkCreateImage(device_.device(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image!");
        }

        VkMemoryRequirements imageMemRequirements;
        vkGetImageMemoryRequirements(device_.device(), image, &imageMemRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = imageMemRequirements.size;
        allocInfo.memoryTypeIndex = device_.findMemoryType(imageMemRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device_.device(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory!");
        }

        vkBindImageMemory(device_.device(), image, imageMemory, 0);
    }
}