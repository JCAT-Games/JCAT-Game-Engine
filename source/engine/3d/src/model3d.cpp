#include "./engine/3D/model3d.h"

namespace JCAT {
    std::vector<VkVertexInputBindingDescription> JCATModel3D::Vertex3D::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> objectBindingDescriptions(1);

        objectBindingDescriptions[0].binding = 0;
        objectBindingDescriptions[0].stride = sizeof(Vertex3D);
        objectBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return objectBindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> JCATModel3D::Vertex3D::getAttributeDescriptions() {
        // We need 2 attribute descriptions: one for color and one for vertices
        std::vector<VkVertexInputAttributeDescription> objectAttributeDescriptions(2);

        objectAttributeDescriptions[0].binding = 0;
        objectAttributeDescriptions[0].location = 0;

        // We need to have 3 inputs for position for 3D objects
        // This is because there are 3 axis for coordinates: x, y, and z
        objectAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        objectAttributeDescriptions[0].offset = offsetof(Vertex3D, position);

        objectAttributeDescriptions[1].binding = 0;
        objectAttributeDescriptions[1].location = 1;
        // We define color with 3 inputs: R, G, and B
        objectAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        objectAttributeDescriptions[1].offset = offsetof(Vertex3D, color);

        return objectAttributeDescriptions;
    }

    JCATModel3D::JCATModel3D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex3D>& objectVertices) : device{d}, resourceManager{r} {
        createVertexBuffers(objectVertices);
    }

    JCATModel3D::JCATModel3D(DeviceSetup& d, ResourceManager& r, const JCATModel3D::ModelBuilder &builder) : device{d}, resourceManager{r} {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    JCATModel3D::~JCATModel3D() {
        vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
        vkFreeMemory(device.device(), vertexBufferMemory, nullptr);

        if (hasIndexBuffer) {
            vkDestroyBuffer(device.device(), indexBuffer, nullptr);
            vkFreeMemory(device.device(), indexBufferMemory, nullptr);
        }
    }

    void JCATModel3D::createVertexBuffers(const std::vector<Vertex3D>& vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());

        // We need to have at least 3 vertices to form a visable shape (like a 2D triange)
        assert(vertexCount >= 3 && "Vertex count must be at least 3!");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        if (useStagingBuffers == false) {
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

    void JCATModel3D::createIndexBuffers(const std::vector<uint32_t>& indices) {
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

    void JCATModel3D::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    
        if (hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void JCATModel3D::draw(VkCommandBuffer commandBuffer) {
        if (hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
        else {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }
}