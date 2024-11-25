#include "./engine/2D/model2d.h"

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

    JCATModel2D::JCATModel2D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex2D>& spriteVertices) : device{d}, resourceManager{r} {
        createVertexBuffers(spriteVertices);
    }

    JCATModel2D::~JCATModel2D() {

    }

    void JCATModel2D::createVertexBuffers(const std::vector<Vertex2D>& vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());

        // We need to have at least 3 vertices to form a visable shape (like a triangle)
        assert(vertexCount >= 3 && "Vertex count must be at least 3!");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
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

    void JCATModel2D::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void JCATModel2D::draw(VkCommandBuffer commandBuffer) {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}