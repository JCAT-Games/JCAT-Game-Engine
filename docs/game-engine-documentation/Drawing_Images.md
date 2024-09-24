# Drawing Images on the screen

To help create a Vulkan game engine, this [youtube playlist](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR) was used.

This file contains the general instructions and setup for getting a triangle to display on the screen.

## Step 1: Create the Vulkan Instance:

The first thing you will need to do in order to utilize Vulkan is to create a Vulkan instance.
The instructions for this can be found in the [Initial Setup File](./Initial_Setup.md).

## Step 2: Physical and Logical Device setup:

You must select the physical device (GPU) that support Vulkan. 
Afterwards, you must create a logical device that interacts with the resources of the physical GPU. 

This instructions for this can be seen in the [Initial Setup File](./Initial_Setup.md).

## Step 3: Window Setup:

Now we must setup the window that we will be drawing and displaying graphics in. This can be done with GLFW.
The instructions for this can also be found in [Initial Setup File](./Initial_Setup.md).

## Step 4: Swap Chain Setup:

Now, you must configure the swap chain. The swap chain is responsible for managing all the images that will be presented on the screen. The swap chain should be configured with double or triple buffering to avoid screen tearing.

The swap chain must be formatted based on the window's resolution and the supported formats.

Instructions on configuring the swap chain can be found in the [Swap Chain README](./Swap_Chain.md).

## Step 5: Image Views Creation:

An image view must be created for each swap chain image. The image view describes how the swap chain images should be interpreted.
Instructions for this can be found in the [Swap Chain README](./Swap_Chain.md).

## Step 6: Render Pass and Framebuffers:

A render pass outlines the sequence of rendering operations.
A framebuffer also must be created for each swap chain image, which binds the image views to the render pass. 

Instructions for configuring both of these can be found in the [Swap Chain README](./Swap_Chain.md).

## Step 7: Graphics Pipeline Creation:

The graphics pipeline defines how vertices are processed and rednered into fragments (pixels) onto the screen. This is one of the most important parts or rendering.

Go to the [Graphics Pipeline README](Graphics-Pipeline_setup.md) for instructions on how to configure the graphics pipeline in Vulkan.

## Step 8: Command Buffers:

Command buffers must be configured for the GPU in order to draw stuff onto the screen.
For drawing a triangle, the following commands must be submitted to the GPU for execution: 

1. Begin a render pass
2. Bind the graphics pipeline
3. Bind vertex buffers containing the triangles vertices
4. Issue a draw command that specifies the number of vertices
5. End the render pass

## Step 9: Configure Synchronization Objects:

Semaphores and fences must be set up to manage synchronization between the CPU and GPU.
A semaphore ensures that an image is availible in the swap chain before rendering starts.
A fence signals when the GPU has completed rendering, allowing the CPU to proceed.

## Step 10: Rendering Loop:

In each frame of rendering:

1. Aquire an image from the swap chain
2. Use a command buffer to record and submit commands for rendering a triangle onto the screen.
3. Present the rendering image onto the screen.

## Step 11: Presentation:

Once command buffer execution is complete, use ```vkQueuePresentKHR``` to present the new image onto the screen.
This swaps the rendered image with the one currently being displayed.
