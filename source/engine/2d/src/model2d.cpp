#include "./engine/2d/model2d.h"

#include <cassert>

namespace JCAT {
    std::vector<VkVertexInputBindingDescription> JCATModel2D::Vertex2D::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> spriteBindingDescriptions(1);

        spriteBindingDescriptions[0].binding = 0;
        spriteBindingDescriptions[0].stride = sizeof(Vertex2D);
        spriteBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return spriteBindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> JCATModel2D::Vertex2D::getAttributeDescriptions() {
        // We need 2 attribute descriptions: one for color and one for vertices
        std::vector<VkVertexInputAttributeDescription> spriteAttributeDescriptions(2);

        spriteAttributeDescriptions[0].binding = 0;
        spriteAttributeDescriptions[0].location = 0;

        // We need to have 2 inputs for position for 2D objects
        // This is because there are 2 axis for coordinates: x and y
        spriteAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        spriteAttributeDescriptions[0].offset = offsetof(Vertex2D, position);

        spriteAttributeDescriptions[1].binding = 0;
        spriteAttributeDescriptions[1].location = 1;
        // We define color with 3 inputs: R, G, and B
        spriteAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        spriteAttributeDescriptions[1].offset = offsetof(Vertex2D, color);

        return spriteAttributeDescriptions;
    }

    /*std::vector<VkVertexInputBindingDescription> JCATModel2D::UIVertex::getBindingDescriptions() {
        return {{
            .binding = 0,
            .stride = sizeof(UIVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }};
    }

    std::vector<VkVertexInputAttributeDescription> JCATModel2D::UIVertex::getAttributeDescriptions() {
        return {
            { // Position
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(UIVertex, position)
            },
            { // UV
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(UIVertex, uv)
            },
            { // Color (normalized UNORM)
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R8G8B8A8_UNORM,
                .offset = offsetof(UIVertex, color)
            }
        };
    }*/

    JCATModel2D::JCATModel2D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex2D>& spriteVertices) : device{d}, resourceManager{r} {
        createVertexBuffers(spriteVertices);
    }

    JCATModel2D::~JCATModel2D() {
        vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
        vkFreeMemory(device.device(), vertexBufferMemory, nullptr);
    }

    void JCATModel2D::createVertexBuffers(const std::vector<Vertex2D>& vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());

        // We need to have at least 3 vertices to form a visable shape (like a triangle)
        assert(vertexCount >= 3 && "Vertex count must be at least 3!");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        if(useStagingBuffers == false) {
            resourceManager.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                vertexBuffer,
                vertexBufferMemory
            );

            void* modelData;
            vkMapMemory(device.device(), vertexBufferMemory, 0, bufferSize, 0, &modelData);
            memcpy(modelData, vertices.data(), static_cast<size_t>(bufferSize));
            vkUnmapMemory(device.device(), vertexBufferMemory);
        }
        else {
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;

            resourceManager.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory
            );

            void* modelData;
            vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &modelData);
            memcpy(modelData, vertices.data(), static_cast<size_t>(bufferSize));
            vkUnmapMemory(device.device(), stagingBufferMemory);

            resourceManager.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                vertexBuffer,
                vertexBufferMemory
            );

            resourceManager.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
        
            vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
            vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
        }
    }

    void JCATModel2D::createIndexBuffers(const std::vector<uint32_t>& indices) {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer) {
            return;
        }

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        
        if (useStagingBuffers == false) {
            resourceManager.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                indexBuffer,
                indexBufferMemory
            );

            void* modelData;
            vkMapMemory(device.device(), indexBufferMemory, 0, bufferSize, 0, &modelData);
            memcpy(modelData, indices.data(), static_cast<size_t>(bufferSize));
            vkUnmapMemory(device.device(), indexBufferMemory);
        }
        else {
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            resourceManager.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory
            );

            void* modelData;
            vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &modelData);
            memcpy(modelData, indices.data(), static_cast<size_t>(bufferSize));
            vkUnmapMemory(device.device(), stagingBufferMemory);

            resourceManager.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                indexBuffer,
                indexBufferMemory
            );

            resourceManager.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

            vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
            vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
        }
    }

    void JCATModel2D::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void JCATModel2D::draw(VkCommandBuffer commandBuffer) {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}