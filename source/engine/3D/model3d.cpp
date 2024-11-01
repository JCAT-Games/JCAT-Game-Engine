#include "model3d.h"

namespace JCAT {
    std::vector<VkVertexInputBindingDescription> JCATModel3D::Vertex::getBindingDescriptions() {

    }

    std::vector<VkVertexInputAttributeDescription> JCATModel3D::Vertex::getAttributeDescriptions() {

    }

    JCATModel3D::JCATModel3D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex>& spriteVertices) : device{d}, resourceManager{r} {

    }

    JCATModel3D::~JCATModel3D() {

    }

    void JCATModel3D::bind(VkCommandBuffer commandBuffer) {

    }

    void JCATModel3D::draw(VkCommandBuffer commandBuffer) {
        
    }
}