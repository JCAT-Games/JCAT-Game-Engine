Attempting to fix the Nvidia GPU bug, I tried to upgrade the command buffer submission process to use `vkQueueSubmit2()` rather than `vkQueueSubmit()`. This attempt was unsuccessful and only causes the program to crash since I believe `vkQueueSubmit2()` requires functionality that the game engine does not currently possess. I also don't think my upgrade would've fixed the Nvidia bug anyways due to finding another issue that often occurs. However, I thought I would add my code here on the off chance that the game engine can use it in the future so that it could be easily readded. This code would go above the `vkQueueSubmit()` function call (which should be replaced by the last line of this code snippet) in [swapchain.cpp](../../source/engine/src/swapChain.cpp) which is located on line 173 at the time of writing this.

```cpp
const int waitArrayLength = sizeof(waitSemaphores)/sizeof(VkSemaphore);
VkSemaphoreSubmitInfo waitSemaphores2[waitArrayLength];
VkPipelineStageFlags2 waitStages2[] = { VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT };
for(int i = 0; i < waitArrayLength; ++i) {
    waitSemaphores2[i].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitSemaphores2[i].pNext = nullptr;
    waitSemaphores2[i].semaphore = waitSemaphores[i];
    waitSemaphores2[i].stageMask = waitStages2[i];
    waitSemaphores2[i].deviceIndex = 0;
    waitSemaphores2[i].value = 1;
}

const int bufferArrayLength = sizeof(buffers)/sizeof(VkCommandBuffer);
VkCommandBufferSubmitInfo buffers2[1];
for(int i = 0; i < bufferArrayLength; ++i) {
    buffers2[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    buffers2[i].pNext = nullptr;
    buffers2[i].commandBuffer = buffers[i];
    buffers2[i].deviceMask = 0;
}

const int signalArrayLength = sizeof(signalSemaphores)/sizeof(VkSemaphore);
VkSemaphoreSubmitInfo signalSemaphores2[1];
VkPipelineStageFlags2 signalStages2[] = { VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT };
for(int i = 0; i < signalArrayLength; ++i) {
    waitSemaphores2[i].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitSemaphores2[i].pNext = nullptr;
    waitSemaphores2[i].semaphore = signalSemaphores[i];
    waitSemaphores2[i].stageMask = signalStages2[i];
    waitSemaphores2[i].deviceIndex = 0;
    waitSemaphores2[i].value = 1;
}
VkSubmitInfo2 submitInfo2 = {};
submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
submitInfo2.pNext = nullptr;
submitInfo2.waitSemaphoreInfoCount = 1;
submitInfo2.pWaitSemaphoreInfos = waitSemaphores2;
submitInfo2.signalSemaphoreInfoCount = 1;
submitInfo2.pSignalSemaphoreInfos = signalSemaphores2;
submitInfo2.commandBufferInfoCount = 1;
submitInfo2.pCommandBufferInfos = buffers2;

//vkResetFences function is called here

if (vkQueueSubmit2(device.graphicsQueue(), 1, &submitInfo2, inFlightFences[currentFrame] != VK_SUCCESS)) {
```