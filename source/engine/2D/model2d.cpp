#include "./engine/2D/model2d.h"

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

    }

    JCATModel2D::~JCATModel2D() {

    }

    void JCATModel2D::bind(VkCommandBuffer commandBuffer) {

    }

    void JCATModel2D::draw(VkCommandBuffer commandBuffer) {

    }
}