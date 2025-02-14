#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <iostream>
#include <fstream>
#include <vector>

#include "./engine/deviceSetup.h"

// Should be declared after deviceSetup in application.cpp

namespace JCAT {

    struct ImageData {
        int width;
        int height;
        int channels;
        std::vector<unsigned char> pixels;
    };

    class ResourceManager {
        public:
            ResourceManager(DeviceSetup& device);

            /** ### Delete Copy, Move, Assignment, and Move Assignment operators ### */
            ResourceManager(const ResourceManager&) = delete;
            void operator=(const ResourceManager&) = delete;
            ResourceManager(ResourceManager&&) = delete;
            ResourceManager& operator=(ResourceManager&&) = delete;

            static std::vector<char> readFile(const std::string& filepath);

            ImageData loadImage(const std::string& filepath, bool flipVertically = true);

            void createBuffer(VkDeviceSize size,
                              VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags properties,
                              VkBuffer& buffer,
                              VkDeviceMemory& bufferMemory);

            VkCommandBuffer beginSingleTimeCommands();
            void endSingleTimeCommands(VkCommandBuffer commandBuffer);
            void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

            void createImageWithInfo(const VkImageCreateInfo& imageInfo,
                                        VkMemoryPropertyFlags properties,
                                        VkImage& image,
                                        VkDeviceMemory& imageMemory);
        private:
            DeviceSetup& device_;
    };
} //JCAT

#endif //RESOURCE_MANAGER_H