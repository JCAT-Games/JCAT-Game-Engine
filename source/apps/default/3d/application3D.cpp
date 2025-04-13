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
#include <random>

namespace JCAT {
    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };

    Application3D::Application3D() {
        globalPool = JCATDescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT) //
            .build();
        loadGameObjects();
    }

    Application3D::~Application3D() {}

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

        std::unique_ptr<JCATDescriptorSetLayout> globalSetLayout = JCATDescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        // For adding textures (We need to add the ability to add mutiple textures in the future)
        Texture texture = Texture(device, resourceManager, "../textures/cobble.png");
        Texture stone = Texture(device, resourceManager, "../textures/close-up-rock-with-lichen.jpg");
        Texture stone2 = Texture(device, resourceManager, "../textures/cracked-plaster-wall.jpg");
        Texture sand = Texture(device, resourceManager, "../textures/metallic-gold-paper-background.jpg");
        Texture wood = Texture(device, resourceManager, "../textures/wood.jpg");
        Texture moss = Texture(device, resourceManager, "../textures/moss.jpg");
        Texture metal = Texture(device, resourceManager, "../textures/metal.jpg");

        // Bind texture to descriptor set
        VkDescriptorImageInfo imageInfo {};
        imageInfo.sampler = stone.getSampler();
        imageInfo.imageView = stone.getImageView();
        imageInfo.imageLayout = stone.getImageLayout();
        
        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < globalDescriptorSets.size(); i++){
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            JCATDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, &imageInfo)
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

    std::unique_ptr<JCATModel3D> createWhiteCubeModel(DeviceSetup& device, ResourceManager& resourceManager, glm::vec3 offset) {
        std::vector<JCATModel3D::Vertex3D> vertices{
            // Left face (X = -0.5)
            {{-.5f, -.5f, -.5f}, {1.0f, 1.0f, 1.0f}, {-1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{-.5f, .5f, .5f}, {1.0f, 1.0f, 1.0f}, {-1.f, 0.f, 0.f}, {1.0f, 1.0f}},
            {{-.5f, -.5f, .5f}, {1.0f, 1.0f, 1.0f}, {-1.f, 0.f, 0.f}, {1.0f, 0.0f}},
            {{-.5f, -.5f, -.5f}, {1.0f, 1.0f, 1.0f}, {-1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{-.5f, .5f, -.5f}, {1.0f, 1.0f, 1.0f}, {-1.f, 0.f, 0.f}, {0.0f, 1.0f}},
            {{-.5f, .5f, .5f}, {1.0f, 1.0f, 1.0f}, {-1.f, 0.f, 0.f}, {1.0f, 1.0f}},
    
            // Right face (X = 0.5)
            {{.5f, -.5f, -.5f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, .5f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f, 0.f}, {1.0f, 1.0f}},
            {{.5f, -.5f, .5f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f, 0.f}, {1.0f, 0.0f}},
            {{.5f, -.5f, -.5f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, -.5f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f, 0.f}, {0.0f, 1.0f}},
            {{.5f, .5f, .5f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f, 0.f}, {1.0f, 1.0f}},
    
            // Top face (Y = -0.5, assuming Y-down)
            {{-.5f, -.5f, -.5f}, {1.0f, 1.0f, 1.0f}, {0.f, -1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, .5f}, {1.0f, 1.0f, 1.0f}, {0.f, -1.f, 0.f}, {1.0f, 1.0f}},
            {{-.5f, -.5f, .5f}, {1.0f, 1.0f, 1.0f}, {0.f, -1.f, 0.f}, {0.0f, 1.0f}},
            {{-.5f, -.5f, -.5f}, {1.0f, 1.0f, 1.0f}, {0.f, -1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, -.5f}, {1.0f, 1.0f, 1.0f}, {0.f, -1.f, 0.f}, {1.0f, 0.0f}},
            {{.5f, -.5f, .5f}, {1.0f, 1.0f, 1.0f}, {0.f, -1.f, 0.f}, {1.0f, 1.0f}},
    
            // Bottom face (Y = 0.5)
            {{-.5f, .5f, -.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, .5f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f, 0.f}, {1.0f, 1.0f}},
            {{-.5f, .5f, .5f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f, 0.f}, {0.0f, 1.0f}},
            {{-.5f, .5f, -.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f, 0.f}, {0.0f, 0.0f}},
            {{.5f, .5f, -.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f, 0.f}, {1.0f, 0.0f}},
            {{.5f, .5f, .5f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f, 0.f}, {1.0f, 1.0f}},
    
            // Front face (Z = 0.5)
            {{-.5f, -.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, 1.f}, {0.0f, 0.0f}},
            {{.5f, .5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, 1.f}, {1.0f, 1.0f}},
            {{-.5f, .5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, 1.f}, {0.0f, 1.0f}},
            {{-.5f, -.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, 1.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, 1.f}, {1.0f, 0.0f}},
            {{.5f, .5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, 1.f}, {1.0f, 1.0f}},
    
            // Back face (Z = -0.5)
            {{-.5f, -.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, -1.f}, {0.0f, 0.0f}},
            {{.5f, .5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, -1.f}, {1.0f, 1.0f}},
            {{-.5f, .5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, -1.f}, {0.0f, 1.0f}},
            {{-.5f, -.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, -1.f}, {0.0f, 0.0f}},
            {{.5f, -.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, -1.f}, {1.0f, 0.0f}},
            {{.5f, .5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f, -1.f}, {1.0f, 1.0f}},
        };
    
        // Just in case, force them to be white again
        for (JCATModel3D::Vertex3D& v : vertices) {
            v.position += offset;
            v.color = {1.0f, 1.0f, 1.0f};  // Make sure every vertex is white
        }
    
        return std::make_unique<JCATModel3D>(device, resourceManager, vertices);
    }

    void Application3D::loadGameObjects() {
        std::shared_ptr<JCATModel3D> cubeModel = createCubeModel(device, resourceManager, { .0f, .0f, .0f });
        std::shared_ptr<JCATModel3D> whiteCubeModel = createWhiteCubeModel(device, resourceManager, { .0f, .0f, .0f });
        std::shared_ptr<JCATModel3D> betterCubeModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/cube.obj", true);
        std::shared_ptr<JCATModel3D> vaseModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/smooth_vase.obj", true);
        std::shared_ptr<JCATModel3D> donutModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/CM_Donut_Scrap.obj", true);
        std::shared_ptr<JCATModel3D> bearModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/3legBear.obj", true);
        std::shared_ptr<JCATModel3D> chairModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/adirondackChair.obj", true);
        std::shared_ptr<JCATModel3D> cacomistleModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/cacomistle.obj", true);
        std::shared_ptr<JCATModel3D> cupModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/cup.obj", true);
        std::shared_ptr<JCATModel3D> deerModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/deer.obj", true);
        std::shared_ptr<JCATModel3D> giraffeModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/giraffe.obj", true);
        std::shared_ptr<JCATModel3D> mongolianGerbilModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/mongolianGerbil.obj", true);
        std::shared_ptr<JCATModel3D> mudpuppyModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/mudpuppy.obj", true);
        std::shared_ptr<JCATModel3D> osakaModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/osaka.obj", true);
        std::shared_ptr<JCATModel3D> penguinModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/penguin.obj", true);
        std::shared_ptr<JCATModel3D> pigModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/pizzaPig.obj", true);
        std::shared_ptr<JCATModel3D> saltChairModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/saltChair.obj", true);
        std::shared_ptr<JCATModel3D> seagullModel = JCATModel3D::createModelFromFile(device, resourceManager, "../models/seagull.obj", true);
	
        GameObject cube = GameObject::createGameObject();
        cube.model3D = cubeModel;
        cube.transform.translation = { .0f, -15.75f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };
        cube.hasLighting = 0;
        cube.hasTexture = 0;
        gameObjects.push_back(std::move(cube));

        GameObject cube2 = GameObject::createGameObject();
        cube2.model3D = cubeModel;
        cube2.transform.translation = { .5f, -15.75f, 4.0f };
        cube2.transform.scale = { 1.0f, 1.0f, 1.0f };
        cube2.hasLighting = 0;
        cube2.hasTexture = 0;
        gameObjects.push_back(std::move(cube2));

        GameObject cube3 = GameObject::createGameObject();
        cube3.model3D = cubeModel;
        cube3.transform.translation = { -.5f, -17.5f, 1.0f };
        cube3.transform.scale = { 1.0f, 0.5f, 1.0f };
        cube3.hasLighting = 0;
        cube3.hasTexture = 0;
        gameObjects.push_back(std::move(cube3));

        GameObject cube4 = GameObject::createGameObject();
        cube4.model3D = cubeModel;
        cube4.transform.translation = { 1.75f, -14.75f, 1.5f };
        cube4.transform.scale = { 1.0f, 0.5f, 1.5f };
        cube4.hasLighting = 0;
        cube4.hasTexture = 0;
        gameObjects.push_back(std::move(cube4));

        GameObject vase = GameObject::createGameObject();
        vase.model3D = vaseModel;
        vase.transform.translation = { -.5f, -18.5f, 1.0f };
        vase.transform.scale = { 1.0f, 1.0f, 1.0f };
        vase.hasLighting = 1;
        vase.hasTexture = 1;
        gameObjects.push_back(std::move(vase));

        GameObject donut = GameObject::createGameObject();
        donut.model3D = donutModel;
        donut.transform.translation = { 1.75f, -18.75f, 1.5f };
        donut.transform.scale = { 1.0f, 1.0f, 1.0f };
        donut.hasLighting = 1;
        donut.hasTexture = 1;
        gameObjects.push_back(std::move(donut));

        float startX = 1.75f;
        float spacing = 10.0f;

        GameObject bear = GameObject::createGameObject();
        bear.model3D = bearModel;
        bear.transform.translation = { startX + 0 * spacing, -15.75f, 10.5f };
        bear.transform.scale = { 1.0f, -1.0f, 1.0f };
        bear.hasLighting = 1;
        bear.hasTexture = 0;
        gameObjects.push_back(std::move(bear));

        GameObject chair = GameObject::createGameObject();
        chair.model3D = chairModel;
        chair.transform.translation = { startX + 1 * spacing, -15.75f, 10.5f };
        chair.transform.scale = { 1.0f, -1.0f, 1.0f };
        chair.hasLighting = 1;
        chair.hasTexture = 0;
        gameObjects.push_back(std::move(chair));

        GameObject cacomistle = GameObject::createGameObject();
        cacomistle.model3D = cacomistleModel;
        cacomistle.transform.translation = { startX + 2 * spacing, -15.75f, 10.5f };
        cacomistle.transform.scale = { 1.0f, -1.0f, 1.0f };
        cacomistle.hasLighting = 1;
        cacomistle.hasTexture = 0;
        gameObjects.push_back(std::move(cacomistle));

        GameObject cup = GameObject::createGameObject();
        cup.model3D = cupModel;
        cup.transform.translation = { startX + 3 * spacing, -15.75f, 10.5f };
        cup.transform.scale = { 1.0f, -1.0f, 1.0f };
        cup.hasLighting = 1;
        cup.hasTexture = 0;
        gameObjects.push_back(std::move(cup));

        GameObject deer = GameObject::createGameObject();
        deer.model3D = deerModel;
        deer.transform.translation = { startX + 4 * spacing, -15.75f, 10.5f };
        deer.transform.scale = { 1.0f, -1.0f, 1.0f };
        deer.hasLighting = 1;
        deer.hasTexture = 0;
        gameObjects.push_back(std::move(deer));

        GameObject giraffe = GameObject::createGameObject();
        giraffe.model3D = giraffeModel;
        giraffe.transform.translation = { startX + 5 * spacing, -15.75f, 10.5f };
        giraffe.transform.scale = { 1.0f, -1.0f, 1.0f };
        giraffe.hasLighting = 1;
        giraffe.hasTexture = 0;
        gameObjects.push_back(std::move(giraffe));

        GameObject mongolianGerbil = GameObject::createGameObject();
        mongolianGerbil.model3D = mongolianGerbilModel;
        mongolianGerbil.transform.translation = { startX + 6 * spacing, -15.75f, 10.5f };
        mongolianGerbil.transform.scale = { 1.0f, -1.0f, 1.0f };
        mongolianGerbil.hasLighting = 1;
        mongolianGerbil.hasTexture = 0;
        gameObjects.push_back(std::move(mongolianGerbil));

        GameObject mudpuppy = GameObject::createGameObject();
        mudpuppy.model3D = mudpuppyModel;
        mudpuppy.transform.translation = { startX + 7 * spacing, -15.75f, 10.5f };
        mudpuppy.transform.scale = { 1.0f, -1.0f, 1.0f };
        mudpuppy.hasLighting = 1;
        mudpuppy.hasTexture = 0;
        gameObjects.push_back(std::move(mudpuppy));

        GameObject osaka = GameObject::createGameObject();
        osaka.model3D = osakaModel;
        osaka.transform.translation = { startX + 8 * spacing, -15.75f, 10.5f };
        osaka.transform.scale = { 1.0f, -1.0f, 1.0f };
        osaka.hasLighting = 1;
        osaka.hasTexture = 0;
        gameObjects.push_back(std::move(osaka));

        GameObject penguin = GameObject::createGameObject();
        penguin.model3D = penguinModel;
        penguin.transform.translation = { startX + 9 * spacing, -15.75f, 10.5f };
        penguin.transform.scale = { 1.0f, -1.0f, 1.0f };
        penguin.hasLighting = 1;
        penguin.hasTexture = 0;
        gameObjects.push_back(std::move(penguin));

        GameObject pig = GameObject::createGameObject();
        pig.model3D = pigModel;
        pig.transform.translation = { startX + 10 * spacing, -15.75f, 10.5f };
        pig.transform.scale = { 1.0f, -1.0f, 1.0f };
        pig.hasLighting = 1;
        pig.hasTexture = 0;
        gameObjects.push_back(std::move(pig));

        GameObject saltChair = GameObject::createGameObject();
        saltChair.model3D = saltChairModel;
        saltChair.transform.translation = { startX + 11 * spacing, -15.75f, 10.5f };
        saltChair.transform.scale = { 1.0f, -1.0f, 1.0f };
        saltChair.hasLighting = 1;
        saltChair.hasTexture = 0;
        gameObjects.push_back(std::move(saltChair));

        GameObject seagull = GameObject::createGameObject();
        seagull.model3D = seagullModel;
        seagull.transform.translation = { startX + 12 * spacing, -15.75f, 10.5f };
        seagull.transform.scale = { 1.0f, -1.0f, 1.0f };
        seagull.hasLighting = 1;
        seagull.hasTexture = 0;
        gameObjects.push_back(std::move(seagull));

        const int TERRAIN_WIDTH = 75;
        const int TERRAIN_DEPTH = 75;
        const int MAX_HEIGHT = 50;
        const float SCALE = 0.01f;
        const float AMPLITUDE = 20.0f;

        std::random_device rd;
        unsigned int seed = rd();

        for (int x = 0; x < TERRAIN_WIDTH; x++) {
            for (int z = 0; z < TERRAIN_DEPTH; z++) {
                PerlinNoise3D object(seed);
                int height = static_cast<int>(PerlinNoise3D::generate3DPerlinNoise(object, x, z, 0.0f, SCALE, AMPLITUDE));

                height = glm::clamp(height, 0, MAX_HEIGHT);

                for (int y = 0; y <= height; y++) {
                    GameObject noiseCube = GameObject::createGameObject();
                    noiseCube.model3D = whiteCubeModel;
                    noiseCube.transform.translation = { x, -y, z };
                    noiseCube.transform.scale = { 1.0f, 1.0f, 1.0f };
                    noiseCube.hasLighting = 1;
                    noiseCube.hasTexture = 1;
                    gameObjects.push_back(std::move(noiseCube));
                }
            }
        }
    }
};