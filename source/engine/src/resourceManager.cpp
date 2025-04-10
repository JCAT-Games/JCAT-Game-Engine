#include <iostream>

#include "./engine/resourceManager.h"

namespace JCAT {
    /// @brief Constructs a ResourceManager object.
    /// @param device Reference to the device setup.
    ResourceManager::ResourceManager(DeviceSetup& device) : device_{ device } {}

    /// @brief Reads a file and returns its contents as a vector of characters.
    /// @param filepath The path to the file.
    /// @return A vector containing the file's contents.
    /// @throws std::runtime_error if the file fails to open.
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

    /// @brief Creates a Vulkan buffer.
    /// @param size The size of the buffer.
    /// @param usage The usage of the buffer.
    /// @param properties The memory properties of the buffer.
    /// @param buffer The buffer to create.
    /// @param bufferMemory The buffer memory to create.
    /// @throws std::runtime_error if the buffer fails to create.
    /// @throws std::runtime_error if the buffer memory fails to allocate.
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

    /// @brief Creates a Vulkan image.
    /// @return The image to create.
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

    /// @brief Ends recording a single time command.
    /// @param commandBuffer The command buffer to end.
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

    /// @brief Copies a buffer to another buffer.
    /// @param srcBuffer The source buffer.
    /// @param dstBuffer The destination buffer.
    /// @param size The size of the buffer.
    void ResourceManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;

        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    /// @brief Creates a Vulkan image.
    /// @param width The width of the image.
    /// @param height The height of the image.
    /// @param buffer The buffer to create the image from.
    /// @param image The image to create.
    /// @param layerCount The number of layers in the image.
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

    /// @brief Creates a Vulkan image.
    /// @param imageInfo The information to create the image.
    /// @param properties The memory properties of the image.
    /// @param image The image to create.
    /// @param imageMemory The image memory to create.
    /// @throws std::runtime_error if the image fails to create.
    /// @throws std::runtime_error if the image memory fails to allocate.
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