#include "./apps/default/3d/application3D.h"

#include "./appCore/keyboardController.h"
#include "./engine/3d/camera3D.h"
#include "./apps/default/3d/application3DRenderer.h"

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
        loadGameObjects();
    }

    Application3D::~Application3D() {}

    void Application3D::run() {
        Application3DRenderer applicationRenderer{ device, resourceManager, renderer.getSwapChainrenderPass() };
    
        Camera3D camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        GameObject viewerObject = GameObject::createGameObject();
        KeyboardController cameraController{};

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
                renderer.beginSwapChainRenderPass(commandBuffer);
                applicationRenderer.renderGameObjects(commandBuffer, gameObjects, camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endRecordingFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }

    // Courtesy of tutorial for this:
    std::unique_ptr<JCATModel3D> createCubeModel(DeviceSetup& device, ResourceManager& resourceManager, glm::vec3 offset) {
        std::vector<JCATModel3D::Vertex3D> vertices{
            // left face (white)
            {{-.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}},
            {{-.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}},
            {{-.5f, -.5f, .5f}, {0.5f, 0.3f, 0.1f}},
            {{-.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}},
            {{-.5f, .5f, -.5f}, {0.5f, 0.3f, 0.1f}},
            {{-.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}},
            {{.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}},
            {{.5f, -.5f, .5f}, {0.5f, 0.3f, 0.1f}},
            {{.5f, -.5f, -.5f}, {0.5f, 0.3f, 0.1f}},
            {{.5f, .5f, -.5f}, {0.5f, 0.3f, 0.1f}},
            {{.5f, .5f, .5f}, {0.5f, 0.3f, 0.1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, .5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.1f, .8f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {0.3f, 0.15f, 0.05f}},
            {{.5f, .5f, .5f}, {0.3f, 0.15f, 0.05f}},
            {{-.5f, .5f, .5f}, {0.3f, 0.15f, 0.05f}},
            {{-.5f, .5f, -.5f}, {0.3f, 0.15f, 0.05f}},
            {{.5f, .5f, -.5f}, {0.3f, 0.15f, 0.05f}},
            {{.5f, .5f, .5f}, {0.3f, 0.15f, 0.05f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {0.6f, 0.4f, 0.2f}},
            {{.5f, .5f, 0.5f}, {0.6f, 0.4f, 0.2f}},
            {{-.5f, .5f, 0.5f}, {0.6f, 0.4f, 0.2f}},
            {{-.5f, -.5f, 0.5f}, {0.6f, 0.4f, 0.2f}},
            {{.5f, -.5f, 0.5f}, {0.6f, 0.4f, 0.2f}},
            {{.5f, .5f, 0.5f}, {0.6f, 0.4f, 0.2f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {0.4f, 0.2f, 0.1f}},
            {{.5f, .5f, -0.5f}, {0.4f, 0.2f, 0.1f}},
            {{-.5f, .5f, -0.5f}, {0.4f, 0.2f, 0.1f}},
            {{-.5f, -.5f, -0.5f}, {0.4f, 0.2f, 0.1f}},
            {{.5f, -.5f, -0.5f}, {0.4f, 0.2f, 0.1f}},
            {{.5f, .5f, -0.5f}, {0.4f, 0.2f, 0.1f}},
        };

        for (JCATModel3D::Vertex3D& v : vertices) {
            v.position += offset;
        }

        return std::make_unique<JCATModel3D>(device, resourceManager, vertices);
    }

    void Application3D::loadGameObjects() {
        std::shared_ptr<JCATModel3D> cubeModel = createCubeModel(device, resourceManager, { .0f, .0f, .0f });
	
        GameObject cube = GameObject::createGameObject();
        cube.model3D = cubeModel;
        cube.transform.translation = { .0f, .0f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };
        gameObjects.push_back(std::move(cube));

        GameObject cube2 = GameObject::createGameObject();
        cube2.model3D = cubeModel;
        cube2.transform.translation = { .5f, .0f, 4.0f };
        cube2.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(cube2));

        GameObject cube3 = GameObject::createGameObject();
        cube3.model3D = cubeModel;
        cube3.transform.translation = { -.5f, -.5f, 1.0f };
        cube3.transform.scale = { 1.0f, 0.5f, 1.0f };
        gameObjects.push_back(std::move(cube3));

        GameObject cube4 = GameObject::createGameObject();
        cube4.model3D = cubeModel;
        cube4.transform.translation = { 1.75f, 0.75f, 1.5f };
        cube4.transform.scale = { 1.0f, 0.5f, 1.5f };
        gameObjects.push_back(std::move(cube4));
    }
};