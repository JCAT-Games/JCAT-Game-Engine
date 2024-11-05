#include "./engine/swapChain.h"

namespace JCAT {
    SwapChain::SwapChain(DeviceSetup& d, VkExtent2D wE, std::string& gameType, bool v) 
    : device{ d }, windowExtent{ wE } {
        type = gameType;
        vsyncEnabled = v;

        init();
    }

    SwapChain::SwapChain(DeviceSetup& d, VkExtent2D wE, std::shared_ptr<SwapChain> previousFrame) 
    : device{ d }, windowExtent{ wE }, previousSwapChain{ previousFrame } {
        init();

        previousSwapChain = nullptr;
    }

    SwapChain::~SwapChain() {

    }

    void SwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }

    bool SwapChain::getVSync() {
        return vsyncEnabled;
    }

    void SwapChain::setVSync(bool toggle) {
        vsyncEnabled = toggle;
    }

    VkResult SwapChain::acquireNextImage(uint32_t* imageIndex) {
        
    }

    VkResult SwapChain::submitSwapChainCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex) {

    }

    void SwapChain::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D swapExtent = chooseSwapExtent(swapChainSupport.capabilities);


    }

    void SwapChain::createImageViews() {

    }

    void SwapChain::createDepthResources() {

    }

    void SwapChain::createRenderPass() {

    }

    void SwapChain::createFramebuffers() {

    }

    void SwapChain::createSyncObjects() {

    }

    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

    }

    VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

    }

    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

    }
}