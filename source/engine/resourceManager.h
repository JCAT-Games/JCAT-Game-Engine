#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <iostream>
#include <fstream>

#include "./engine/deviceSetup.h"

// Should be declared after deviceSetup in application.cpp

namespace JCAT {
    /**
     * @class ResourceManager
     * @brief Manager for buffers and images to be used by JCAT Game Engine
     * 
     * This class contains functions to create and copy Vulkan buffers and images.
     * Allows for recording commands to a Vulkan command buffer object, copying
     * buffer contents to other buffers and images, and reading files.
     */
    class ResourceManager {
        public:
            /**
             * Constructs a ResourceManager object
             * @param device Reference to the device object to use in creating resources
             */
            ResourceManager(DeviceSetup& device);

            //Delete Copy, Move, Assignment, and Move Assignment operators
            ResourceManager(const ResourceManager&) = delete;
            void operator=(const ResourceManager&) = delete;
            ResourceManager(ResourceManager&&) = delete;
            ResourceManager& operator=(ResourceManager&&) = delete;

            /**
             * Reads the given file and returns its contents as a vector of chars
             * @param filepath The path to the file
             * @throws std::runtime_error if file fails to open
             * @return A vector of strings containing the file's contents
             */
            static std::vector<char> readFile(const std::string& filepath);

            /**
             * Creates a Vulkan vertex buffer
             * @param size The size of the buffer
             * @param usage The usage flags for the buffer
             * @param properties The memory property flags for the buffer
             * @param buffer Reference to the object in which the new buffer is made
             * @param bufferMemory Reference to the object in which information about
             *                     the allocated memory is returned
             * @throws std::runtime_error if the vertex buffer cannot be created 
             *         or memory cannot be allocated for the buffer
             */
            void createBuffer(VkDeviceSize size,
                              VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags properties,
                              VkBuffer& buffer,
                              VkDeviceMemory& bufferMemory);
            
            /**
             * Creates and returns a command buffer
             * @returns The command buffer currently being recorded to
             */
            VkCommandBuffer beginSingleTimeCommands();

            /**
             * Ends recording for the given command buffer and submits graphics queue
             * @param commandBuffer The command buffer to end recording for
             */
            void endSingleTimeCommands(VkCommandBuffer commandBuffer);

            /**
             * Copies the contents from srcBuffer to dstBuffer
             * @param srcBuffer The source buffer to copy contents from
             * @param dstBuffer The destination buffer to copy contents into
             * @param size The number of bytes to copy
             */
            void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

            /**
             * Copies the contents of the buffer to the specified image
             * @param buffer The source buffer to copy content from
             * @param image The destination image to copy content into
             * @param width The width of the image
             * @param height The height of the image
             * @param layerCount The number of layers to copy
             */
            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

            /**
             * Creates a Vulkan image in the specified object with using imageInfo
             * @param imageInfo Reference to structure containing parameters to be used
             * @param properties The memory property flags for the image
             * @param image Reference to the object in which the new image is made
             * @param imageMemory Reference to the object in which information about
             *                    the allocated memory is returned
             * @throws std::runtime_error if image could not be created or
             *         memory could not be allocated for the image
             */
            void createImageWithInfo(const VkImageCreateInfo& imageInfo,
                                        VkMemoryPropertyFlags properties,
                                        VkImage& image,
                                        VkDeviceMemory& imageMemory);
        private:
            //The device to use for working with resources
            DeviceSetup& device_;
    };
} //JCAT

#endif //RESOURCE_MANAGER_H