#include <iostream>

#include "resourceManager.h"

namespace JCAT {
    ResourceManager::ResourceManager(DeviceSetup& device) : device_{ device } {

    }

    void ResourceManager::createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory) {

    }

    VkCommandBuffer ResourceManager::beginSingleTimeCommands() {

    }

    void ResourceManager::endSingleTimeCommands(VkCommandBuffer commandBuffer) {

    }

    void ResourceManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

    }

    void ResourceManager::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {

    }

    void ResourceManager::createImageWithInfo(
        const VkImageCreateInfo& imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory) {

    }
}