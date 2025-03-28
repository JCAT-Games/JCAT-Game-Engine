#include "./apps/default/3d/application3D.h"

#include "./appCore/keyboardController.h"
#include "./engine/3d/camera3D.h"
#include "./apps/default/3d/application3DRenderer.h"
#include "./apps/default/3d/perlinNoise3D.h"
#include "./engine/buffer.h"

#include "./engine/texture.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>

namespace JCAT {
    Application3D::Application3D() {
        globalPool = JCATDescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT) //
            .build();
        loadGameObjects();
    }

    Application3D::~Application3D() {}

    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };

    void Application3D::run() {

        // Create and map global uniform buffers
        std::vector<std::unique_ptr<JCATBuffer> > uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < uboBuffers.size(); i++){
            uboBuffers[i] = std::make_unique<JCATBuffer>(
                device,
                resourceManager,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

        auto globalSetLayout = JCATDescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) //
            .build();

        //
        Texture texture = Texture(device, resourceManager, "../textures/image.jpg");

        VkDescriptorImageInfo imageInfo {};
        imageInfo.sampler = texture.getSampler();
        imageInfo.imageView = texture.getImageView();
        imageInfo.imageLayout = texture.getImageLayout();
        //
        
        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < globalDescriptorSets.size(); i++){
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            JCATDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, &imageInfo) //
                .build(globalDescriptorSets[i]);
        }

        Application3DRenderer applicationRenderer{ 
            device,
            resourceManager,
            renderer.getSwapChainrenderPass(),
            globalSetLayout->getDescriptorSetLayout() 
        };
    
        Camera3D camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        GameObject viewerObject = GameObject::createGameObject();
        KeyboardController cameraController{};
        cameraController.inFullscreen = window.windowInFullscreen();

        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();

        while (!window.shouldWindowClose()) {
            glfwPollEvents();

            std::chrono::time_point<std::chrono::high_resolution_clock> newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveObjectInPlaneXZ(window.getWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

            if (VkCommandBuffer commandBuffer = renderer.beginRecordingFrame()) {

                // Create new FrameInfo object that stores relevant frame information
                int frameIndex = renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };

                // update uniform buffers
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                renderer.beginSwapChainRenderPass(commandBuffer);
                applicationRenderer.renderGameObjects(frameInfo, gameObjects);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endRecordingFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }

    // Courtesy of tutorial for this:
    std::unique_ptr<JCATModel3D> createCubeModel(DeviceSetup& device, ResourceManager& resourceManager, glm::vec3 offset) {
        std::vector<JCATModel3D::Vertex3D> vertices{
            // Left face (X = -0.5)
            {{-.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}, {-1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{-.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}, {-1.f, 0.f, 0.f}, {1.0f, 1.0f}},
            {{-.5f, -.5f, .5f}, {0.5f, 0.3f, 0.1f}, {-1.f, 0.f, 0.f}, {1.0f, 0.0f}},
            {{-.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}, {-1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{-.5f, .5f, -.5f}, {0.5f, 0.3f, 0.1f}, {-1.f, 0.f, 0.f}, {0.0f, 1.0f}},
            {{-.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}, {-1.f, 0.f, 0.f}, {1.0f, 1.0f}},
    
            // Right face (X = 0.5)
            {{.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}, {1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}, {1.f, 0.f, 0.f}, {1.0f, 1.0f}},
            {{.5f, -.5f, .5f}, {0.5f, 0.3f, 0.1f}, {1.f, 0.f, 0.f}, {1.0f, 0.0f}},
            {{.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}, {1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, -.5f}, {0.5f, 0.3f, 0.1f}, {1.f, 0.f, 0.f}, {0.0f, 1.0f}},
            {{.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}, {1.f, 0.f, 0.f}, {1.0f, 1.0f}},
    
            // Top face (Y = -0.5, assuming Y-down)
            {{-.5f, -.5f, -.5f}, {.1f, .8f, .1f}, {0.f, -1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, .5f}, {.1f, .8f, .1f}, {0.f, -1.f, 0.f}, {1.0f, 1.0f}},
            {{-.5f, -.5f, .5f}, {.1f, .8f, .1f}, {0.f, -1.f, 0.f}, {0.0f, 1.0f}},
            {{-.5f, -.5f, -.5f}, {.1f, .8f, .1f}, {0.f, -1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, -.5f}, {.1f, .8f, .1f}, {0.f, -1.f, 0.f}, {1.0f, 0.0f}},
            {{.5f, -.5f, .5f}, {.1f, .8f, .1f}, {0.f, -1.f, 0.f}, {1.0f, 1.0f}},
    
            // Bottom face (Y = 0.5)
            {{-.5f, .5f, -.5f}, {0.3f, 0.15f, 0.05f}, {0.f, 1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, .5f}, {0.3f, 0.15f, 0.05f}, {0.f, 1.f, 0.f}, {1.0f, 1.0f}},
            {{-.5f, .5f, .5f}, {0.3f, 0.15f, 0.05f}, {0.f, 1.f, 0.f}, {0.0f, 1.0f}},
            {{-.5f, .5f, -.5f}, {0.3f, 0.15f, 0.05f}, {0.f, 1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, -.5f}, {0.3f, 0.15f, 0.05f}, {0.f, 1.f, 0.f}, {1.0f, 0.0f}},
            {{.5f, .5f, .5f}, {0.3f, 0.15f, 0.05f}, {0.f, 1.f, 0.f}, {1.0f, 1.0f}},
    
            // Front face (Z = 0.5)
            {{-.5f, -.5f, 0.5f}, {0.6f, 0.4f, 0.2f}, {0.f, 0.f, 1.f}, {0.0f, 0.0f}},
            {{.5f, .5f, 0.5f}, {0.6f, 0.4f, 0.2f}, {0.f, 0.f, 1.f}, {1.0f, 1.0f}},
            {{-.5f, .5f, 0.5f}, {0.6f, 0.4f, 0.2f}, {0.f, 0.f, 1.f}, {0.0f, 1.0f}},
            {{-.5f, -.5f, 0.5f}, {0.6f, 0.4f, 0.2f}, {0.f, 0.f, 1.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, 0.5f}, {0.6f, 0.4f, 0.2f}, {0.f, 0.f, 1.f}, {1.0f, 0.0f}},
            {{.5f, .5f, 0.5f}, {0.6f, 0.4f, 0.2f}, {0.f, 0.f, 1.f}, {1.0f, 1.0f}},
    
            // Back face (Z = -0.5)
            {{-.5f, -.5f, -0.5f}, {0.4f, 0.2f, 0.1f}, {0.f, 0.f, -1.f}, {0.0f, 0.0f}},
            {{.5f, .5f, -0.5f}, {0.4f, 0.2f, 0.1f}, {0.f, 0.f, -1.f}, {1.0f, 1.0f}},
            {{-.5f, .5f, -0.5f}, {0.4f, 0.2f, 0.1f}, {0.f, 0.f, -1.f}, {0.0f, 1.0f}},
            {{-.5f, -.5f, -0.5f}, {0.4f, 0.2f, 0.1f}, {0.f, 0.f, -1.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, -0.5f}, {0.4f, 0.2f, 0.1f}, {0.f, 0.f, -1.f}, {1.0f, 0.0f}},
            {{.5f, .5f, -0.5f}, {0.4f, 0.2f, 0.1f}, {0.f, 0.f, -1.f}, {1.0f, 1.0f}},
        };

        for (JCATModel3D::Vertex3D& v : vertices) {
            v.position += offset;
        }

        return std::make_unique<JCATModel3D>(device, resourceManager, vertices);
    }

    void Application3D::loadGameObjects() {
        std::shared_ptr<JCATModel3D> cubeModel = createCubeModel(device, resourceManager, { .0f, .0f, .0f });
        std::shared_ptr<JCATModel3D> vaseModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/smooth_vase.obj", true);
        std::shared_ptr<JCATModel3D> donutModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/cube.obj", true);
	
        GameObject cube = GameObject::createGameObject();
        cube.model3D = cubeModel;
        cube.transform.translation = { .0f, .0f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };
        cube.hasLighting = 0;
        gameObjects.push_back(std::move(cube));

        GameObject cube2 = GameObject::createGameObject();
        cube2.model3D = cubeModel;
        cube2.transform.translation = { .5f, .0f, 4.0f };
        cube2.transform.scale = { 1.0f, 1.0f, 1.0f };
        cube2.hasLighting = 0;
        gameObjects.push_back(std::move(cube2));

        GameObject cube3 = GameObject::createGameObject();
        cube3.model3D = cubeModel;
        cube3.transform.translation = { -.5f, -.5f, 1.0f };
        cube3.transform.scale = { 1.0f, 0.5f, 1.0f };
        cube3.hasLighting = 0;
        gameObjects.push_back(std::move(cube3));

        GameObject cube4 = GameObject::createGameObject();
        cube4.model3D = cubeModel;
        cube4.transform.translation = { 1.75f, 0.75f, 1.5f };
        cube4.transform.scale = { 1.0f, 0.5f, 1.5f };
        cube4.hasLighting = 0;
        gameObjects.push_back(std::move(cube4));

        GameObject vase = GameObject::createGameObject();
        vase.model3D = vaseModel;
        vase.transform.translation = { -.5f, -10.5f, 1.0f };
        vase.transform.scale = { 1.0f, 1.0f, 1.0f };
        vase.hasLighting = 1;
        gameObjects.push_back(std::move(vase));

        GameObject donut = GameObject::createGameObject();
        donut.model3D = donutModel;
        donut.transform.translation = { 1.75f, -10.75f, 1.5f };
        donut.transform.scale = { 1.0f, 1.0f, 1.0f };
        donut.hasLighting = 1;
        gameObjects.push_back(std::move(donut));

        const int TERRAIN_WIDTH = 20;
        const int TERRAIN_DEPTH = 20;
        const int MAX_HEIGHT = 30;
        const float SCALE = 0.1f;
        const float AMPLITUDE = 10.0f;

        for (int x = 0; x < TERRAIN_WIDTH; x++) {
            for (int z = 0; z < TERRAIN_DEPTH; z++) {
                PerlinNoise3D object;
                int height = static_cast<int>(PerlinNoise3D::generate3DPerlinNoise(object, x, z, 0.0f, SCALE, AMPLITUDE));

                height = glm::clamp(height, 0, MAX_HEIGHT);

                for (int y = 0; y <= height; y++) {
                    GameObject noiseCube = GameObject::createGameObject();
                    noiseCube.model3D = cubeModel;
                    noiseCube.transform.translation = { x, -y, z };
                    noiseCube.transform.scale = { 1.0f, 1.0f, 1.0f };
                    noiseCube.hasLighting = 0;
                    gameObjects.push_back(std::move(noiseCube));
                }
            }
        }
    }
};