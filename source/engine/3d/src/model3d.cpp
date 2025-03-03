#include "./engine/3D/model3d.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

namespace std {
	template <>
	struct hash<JCAT::JCATModel3D::Vertex3D> {
		size_t operator()(JCAT::JCATModel3D::Vertex3D const& vertex) const {
			size_t seed = 0;
			JCAT::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

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

    bool JCATModel3D::Vertex3D::operator==(const Vertex3D& other) const {
        return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
    }

    void JCATModel3D::ModelBuilder::loadModel(const std::string& filepath, bool hasIndexBuffer) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warning;
        std::string error;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filepath.c_str())) {
            throw std::runtime_error(warning + error);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex3D, uint32_t> uniqueVertices{};
        for (const tinyobj::shape_t& shape : shapes) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {
                Vertex3D vertex{};

                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                    
                    int colorIndex = 3 * index.vertex_index + 2;
                    if (colorIndex < attrib.colors.size()) {
                        vertex.color = {
                            attrib.colors[colorIndex - 2],
                            attrib.colors[colorIndex - 1],
                            attrib.colors[colorIndex - 0],
                        };
                    }
                    else {
                        vertex.color = { 1.f, 1.f, 1.f };
                    }
                }

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.normals[2 * index.texcoord_index + 0],
                        attrib.normals[2 * index.texcoord_index + 1]
                    };
                }

                if (!hasIndexBuffer) {
                    vertices.push_back(vertex);
                }
                else {
                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }
                    
                    indices.push_back(uniqueVertices[vertex]);
                }
            }
        }
    }

    JCATModel3D::JCATModel3D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex3D>& objectVertices) : device{d}, resourceManager{r} {
        hasIndexBuffer = false;
        createVertexBuffers(objectVertices);
    }

    JCATModel3D::JCATModel3D(DeviceSetup& d, ResourceManager& r, const JCATModel3D::ModelBuilder &builder) : device{d}, resourceManager{r} {
        hasIndexBuffer = true;
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

    std::unique_ptr<JCATModel3D> JCATModel3D::createModelFromFile(DeviceSetup& device, ResourceManager& resourceManager, const std::string& filepath, bool hasIndexBuffers) {
        ModelBuilder builder{};
        builder.loadModel(filepath, hasIndexBuffers);

        return std::make_unique<JCATModel3D>(device, resourceManager, builder);
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