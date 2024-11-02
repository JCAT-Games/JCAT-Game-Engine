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

    JCATModel3D::JCATModel3D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex3D>& spriteVertices) : device{d}, resourceManager{r} {
        
    }

    JCATModel3D::~JCATModel3D() {

    }

    void JCATModel3D::bind(VkCommandBuffer commandBuffer) {

    }

    void JCATModel3D::draw(VkCommandBuffer commandBuffer) {
        
    }
}