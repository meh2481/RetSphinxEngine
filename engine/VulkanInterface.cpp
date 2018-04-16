#include "VulkanInterface.h"
#include "Logger.h"

#include <SDL.h>
//#include <SDL_vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>

//Application-specific defines
#define WIDTH 800
#define HEIGHT 600
#define APPLICATION_NAME "Vulkan SDL"

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define POINT_VERSION 0

//Vulkan-specific defines
#define VULKAN_API_VERSION VK_API_VERSION_1_1
#define QUEUE_PRIORITY 1.0f

//Temp resources
#define VERT_SHADER "res/shaders/vert.spv"
#define FRAG_SHADER "res/shaders/frag.spv"
#define IMG_TEXTURE "res/gfx/blob2.png"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<Vertex> vertices = {
    { { -0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
    { { 0.5f, 0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
    { { -0.5f, 0.5f, -0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

    { { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
    { { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

#ifdef ENABLE_VALIDATION_LAYERS
//Enable validation layers in debug mode

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

VkResult createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if(func != NULL)
        return func(instance, pCreateInfo, pAllocator, pCallback);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if(func != NULL)
        func(instance, callback, pAllocator);
}
#endif

#ifdef ENABLE_VALIDATION_LAYERS
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{

    LOG(ERR) << "Validation layer: " << msg;

    return VK_FALSE;
}

VulkanInterface::VulkanInterface(SDL_Window* w)
{
    window = w;
    initVulkan();
}

VulkanInterface::~VulkanInterface()
{
    cleanup();
}

void VulkanInterface::setupDebugCallback()
{
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugCallback;

    if(createDebugReportCallbackEXT(instance, &createInfo, NULL, &callback) != VK_SUCCESS)
    {
        LOG(ERR) << "failed to set up debug callback";
        assert(false);
    }
}
#endif

std::vector<char> VulkanInterface::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        LOG(ERR) << "Failed to open file " << filename.c_str();
        assert(false);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

void VulkanInterface::initVulkan()
{
    createInstance();
#ifdef ENABLE_VALIDATION_LAYERS
    setupDebugCallback();
#endif
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createSwapChainImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();

    createCommandPool();
    createDepthResources();
    createFramebuffers();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    createVertIndexBuffers();
    createUniformBuffer();
    createDescriptorPool();
    createDescriptorSet();
    createCommandBuffers();
    createSemaphores();
    createFences();
}

void VulkanInterface::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();

    createImage(swapChainExtent.width, swapChainExtent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

VkFormat VulkanInterface::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for(VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    LOG(ERR) << "Failed to find supported format";
    assert(false);
    return VK_FORMAT_UNDEFINED; //Keep compiler happy
}

VkFormat VulkanInterface::findDepthFormat()
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool VulkanInterface::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanInterface::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;   //VK_FILTER_NEAREST for pixelly image
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;     //Possible config option for performance
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = (float)textureMipLevels;

    if(vkCreateSampler(device, &samplerInfo, NULL, &textureSampler) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create texture sampler";
        assert(false);
    }
}

void VulkanInterface::createTextureImageView()
{
    textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, textureMipLevels);
}

VkImageView VulkanInterface::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if(vkCreateImageView(device, &viewInfo, NULL, &imageView) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create texture image view";
        assert(false);
    }

    return imageView;
}

void VulkanInterface::createTextureImage()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(IMG_TEXTURE, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if(!pixels)
    {
        LOG(ERR) << "Failed to load texture image";
        assert(false);
    }

    textureMipLevels = (uint32_t)std::floor(std::log2(std::max(texWidth, texHeight))) + 1;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);

    stbi_image_free(pixels);

    //TODO: VK_FORMAT_BC1_RGBA_UNORM_BLOCK for DXT-compressed images
    createImage(texWidth, texHeight, textureMipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureMipLevels);
    copyBufferToImage(stagingBuffer, textureImage, (uint32_t)texWidth, (uint32_t)texHeight);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    generateMipmaps(textureImage, texWidth, texHeight, textureMipLevels);
}

void VulkanInterface::generateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for(uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            NULL,
            0,
            NULL,
            1,
            &barrier);

        VkImageBlit blit = {};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth / 2, mipHeight / 2, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            NULL,
            0,
            NULL,
            1,
            &barrier);

        if(mipWidth > 1) mipWidth /= 2;
        if(mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &barrier);

    endSingleTimeCommands(commandBuffer);
}

void VulkanInterface::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;  //TODO VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK and VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateImage(device, &imageInfo, NULL, &image) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create image";
        assert(false);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(device, &allocInfo, NULL, &imageMemory) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to allocate image memory!";
        assert(false);
    }

    vkBindImageMemory(device, image, imageMemory, 0);
}

VkCommandBuffer VulkanInterface::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanInterface::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void VulkanInterface::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;

    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if(hasStencilComponent(format))
        {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        LOG(ERR) << "Unsupported layout transition";
        assert(false);
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void VulkanInterface::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(commandBuffer);
}

void VulkanInterface::createDescriptorSet()
{
    //Create the descriptor set
    VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)  //Note: Automagically freed
    {
        LOG(ERR) << "Failed to allocate descriptor set";
        assert(false);
    }

    //Create descriptors
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(RenderState);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = textureSampler;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, NULL);
}

void VulkanInterface::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create descriptor pool";
        assert(false);
    }
}

void VulkanInterface::createUniformBuffer()
{
    VkDeviceSize bufferSize = sizeof(RenderState);
    createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
}

void VulkanInterface::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = NULL;

    //TODO: Multiple descriptor sets via layout(set = 0, binding = 0) uniform RenderState { ... }
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    if(vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create descriptor set layout";
        assert(false);
    }
}

void VulkanInterface::createVertIndexBuffers()
{
    //Combined buffer size
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    VkDeviceSize vertBufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize bufferSize = indexBufferSize + vertBufferSize;

    //Create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    //Copy in data
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    //Index data first
    memcpy(data, indices.data(), (size_t)indexBufferSize);
    //Vertex data after index data
    memcpy((void*)((VkDeviceSize)data+indexBufferSize), vertices.data(), (size_t)vertBufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    //Create buffer (Used as both index buffer and vertex buffer, so set both flags accordingly)
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, combinedBuffer, combinedBufferMemory);

    copyBuffer(stagingBuffer, combinedBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}

void VulkanInterface::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(device, &bufferInfo, NULL, &buffer) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create vertex buffer";
        assert(false);
    }

    //Find memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    //Allocate memory
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    //TODO: You're not supposed to actually call vkAllocateMemory for every individual buffer. Split up allocations between many different objects using offsets.
    //See https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator for a possible allocator to use
    if(vkAllocateMemory(device, &allocInfo, NULL, &bufferMemory) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to allocate vertex buffer memory";
        assert(false);
    }

    //Bind memory
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void VulkanInterface::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

uint32_t VulkanInterface::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    LOG(ERR) << "Failed to find suitable memory type";
    assert(false);
    return 0;   //Keep compiler happy
}

void VulkanInterface::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if(vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderFinishedSemaphore) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create semaphores";
        assert(false);
    }
}

void VulkanInterface::createFences()
{
    size_t fenceCount = swapChainImages.size();
    fences.resize(fenceCount);
    bufferSubmitted.resize(fenceCount);

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;    //Start unsignaled

    for(size_t i = 0; i < fenceCount; i++)
    {
        if(vkCreateFence(device, &fenceInfo, NULL, &fences.data()[i]) != VK_SUCCESS)
        {
            LOG(ERR) << "Failed to create fences";
            assert(false);
        }
        bufferSubmitted[i] = false;
    }
}

void VulkanInterface::createCommandBuffers()
{
    //Allocate command buffer
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to allocate command buffers";
        assert(false);
    }
}

void VulkanInterface::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;   //Allow us to reset command buffers from this pool

    if(vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create command pool";
        assert(false);
    }
}

void VulkanInterface::createFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());
    for(size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
            swapChainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(device, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            LOG(ERR) << "Failed to create framebuffer";
            assert(false);
        }
    }
}

void VulkanInterface::createRenderPass()
{
    //Attachment description
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //Create subpass attachment
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //Create depth attachment
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //Create single subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    //Create subpass dependencies
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;    //Wait for the color attachment output stage itself
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    //Create render pass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create render pass";
        assert(false);
    }
}

void VulkanInterface::createGraphicsPipeline()
{
    std::vector<char> vertShaderCode = readFile(VERT_SHADER);
    std::vector<char> fragShaderCode = readFile(FRAG_SHADER);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    //Vert shader stage
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    //Frag shader stage
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    //Create shader stages
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    //Vertex input
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    //Input assembly: triangles in a list of vertices
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    //Viewport assembly
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    //Scissor rect
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    //Viewport state
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    //Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    //Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = NULL;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    //Depth stencil
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;

    //Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    //colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    //Alpha blending
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    //Fill in state dynamically
    /*
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
    */

    //Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = NULL;

    if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create pipeline layout";
        assert(false);
    }

    //Create pipeline!
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;    //Vert & frag
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = NULL;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create graphics pipeline!";
        assert(false);
    }

    vkDestroyShaderModule(device, fragShaderModule, NULL);
    vkDestroyShaderModule(device, vertShaderModule, NULL);
}

VkShaderModule VulkanInterface::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create shader module";
        assert(false);
    }
    return shaderModule;
}

void VulkanInterface::createSwapChainImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for(uint32_t i = 0; i < swapChainImages.size(); i++)
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void VulkanInterface::recreateSwapChain()
{
    cleanupSwapChain();

    createSwapChain();
    createSwapChainImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createDepthResources();
    createFramebuffers();
    createCommandBuffers();
}

void VulkanInterface::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    //Determine optimal number of images in the swapchain
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    //Create the swapchain object
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

    if(indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = NULL;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(device, &createInfo, NULL, &swapChain) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create swap chain";
        assert(false);
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, NULL);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    //Store format & extent
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void VulkanInterface::createSurface()
{
    if(!SDL_Vulkan_CreateSurface(window, instance, &surface))
    {
        LOG(ERR) << "Couldn\'t create window surface: " << SDL_GetError();
        assert(false);
    }
}

void VulkanInterface::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = QUEUE_PRIORITY;   //We probably won't care about queue priority ever
    for(int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE; //Config option: Not require this

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
#ifdef ENABLE_VALIDATION_LAYERS
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to create logical device!";
        assert(false);
    }

    vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
}

void VulkanInterface::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if(deviceCount == 0)
    {
        LOG(ERR) << "No physical device has Vulkan support";
        assert(false);
    }
    LOG(INFO) << "Found " << deviceCount << " physical device(s)";
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    //Find suitable device
    std::multimap<int, VkPhysicalDevice> candidates;

    for(const auto& device : devices)
    {
        //TODO: Let user select device as a config option, or better yet parellize across all GPUs
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if(candidates.rbegin()->first > 0)
        physicalDevice = candidates.rbegin()->second;
    else
    {
        LOG(ERR) << "Failed to find a suitable GPU";
        assert(false);
    }
}

int VulkanInterface::rateDeviceSuitability(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    LOG(INFO) << "Device name: " << deviceProperties.deviceName;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 2048;  //Arbitrary increase in score so that discrete GPUs are favored more highly

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if(!deviceFeatures.geometryShader || !deviceFeatures.samplerAnisotropy) //Config option: Don't require ansitropic filtering
        return 0;

    //Need device to be able to process commands we want to use
    QueueFamilyIndices indices = findQueueFamilies(device);
    if(!indices.isComplete())
        return 0;

    //Need device to have swapchain support (generally true if presentation queue is supported)
    if(!checkDeviceExtensionSupport(device))
        return 0;

    //Swapchain needs to have at least one format and one present mode
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        return 0;

    return score;
}

VkSurfaceFormatKHR VulkanInterface::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    //No preferred format; use the best one
    if(availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
        return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    //See if our most preferred combination is available
    for(const auto& availableFormat : availableFormats)
    {
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }

    //First format is generally the best from there
    return availableFormats[0];
}

VkPresentModeKHR VulkanInterface::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
    //Double-buffering with wait is the only guaranteed mode
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for(const auto& availablePresentMode : availablePresentModes)
    {
        //Use triple buffering if available
        if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    return bestMode;
}

VkExtent2D VulkanInterface::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    else
    {
        int width = WIDTH;
        int height = HEIGHT;
        SDL_Vulkan_GetDrawableSize(window, &width, &height);
        VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

bool VulkanInterface::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for(const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanInterface::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details = {};

    //Query surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if(formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    //Query supported presentation modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if(presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

QueueFamilyIndices VulkanInterface::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for(const auto& queueFamily : queueFamilies)
    {
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(queueFamily.queueCount > 0 && presentSupport)
            indices.presentFamily = i;

        if(indices.isComplete())
            break;

        i++;
    }

    return indices;
}

void VulkanInterface::mainLoop(const RenderState& state)
{
    //Make a copy so we can flip y
    RenderState ubo = {state.model, state.view, state.proj};
    ubo.proj[1][1] *= -1; //Flip y

    //Update uniforms
    updateUniformBuffer(ubo);

    //Draw
    drawFrame();
}

//TODO: Look into push constants instead for a more efficient way to pass frequently-changing values to the shader
void VulkanInterface::updateUniformBuffer(const RenderState& ubo)
{
    //Get time in seconds since program start
    float time = (float)SDL_GetTicks() / 1000.0f;

    //Copy memory
    void* data;
    vkMapMemory(device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, uniformBufferMemory);
}

void VulkanInterface::resizeWindow(int width, int height)
{
    recreateSwapChain();
}

void VulkanInterface::drawFrame()
{
    //Get a new image from the swapchain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        //Out of date; recreate swapchain and cancel this draw pass
        recreateSwapChain();
        return;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LOG(ERR) << "Failed to acquire swap chain image";
        assert(false);
    }

    //Submit the command buffer
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //First-run check to prevent validation layer errors
    if(bufferSubmitted[imageIndex])
    {
        //Make absolutely sure that the work has completed, via fence wait
        if(vkWaitForFences(device, 1, &fences[imageIndex], true, UINT64_MAX) != VK_SUCCESS)
        {
            LOG(ERR) << "Failed to wait for fence";
            assert(false);
        }
    }
    bufferSubmitted[imageIndex] = true;

    //Reset the fences we waited on, so they can be re-used
    if(vkResetFences(device, 1, &fences[imageIndex]) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to reset fence";
        assert(false);
    }

    //TODO: Change vertex data per-frame here
    setupCommandBuffer(imageIndex); //Rebuild the command buffer, since the vertex buffer data has changed

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, fences[imageIndex]) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to submit draw command buffer";
        assert(false);
    }

    //Presentation
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    //Recreate swapchain if needed or suboptimal
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        recreateSwapChain();
    else if(result != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to present swap chain image";
        assert(false);
    }

#ifdef ENABLE_VALIDATION_LAYERS
    //Fix memory leak in validation layer by explicitly syncronizing with GPU
    vkQueueWaitIdle(presentQueue);
#endif
}

void VulkanInterface::setupCommandBuffer(uint32_t index)
{
    if(vkResetCommandBuffer(commandBuffers[index], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS)
    {
        LOG(ERR) << "Unable to reset command buffer";
        assert(false);
    }

    //Start buffer recording
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = NULL;

    vkBeginCommandBuffer(commandBuffers[index], &beginInfo);

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    //Start render pass
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[index];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Draw
    vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkBuffer vertexBuffers[] = { combinedBuffer };
    VkDeviceSize offsets[] = { sizeof(indices[0]) * indices.size() };   //Vertex buffer after index buffer in data
    vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers[index], combinedBuffer, 0, VK_INDEX_TYPE_UINT16);

    //Bind descriptor sets
    vkCmdBindDescriptorSets(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

    vkCmdDrawIndexed(commandBuffers[index], (uint32_t)indices.size(), 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffers[index]);

    if(vkEndCommandBuffer(commandBuffers[index]) != VK_SUCCESS)
    {
        LOG(ERR) << "Failed to record command buffer";
        assert(false);
    }
}

void VulkanInterface::cleanupSwapChain()
{
    //Wait until everything is done
    vkDeviceWaitIdle(device);

    vkDestroyImageView(device, depthImageView, NULL);
    vkDestroyImage(device, depthImage, NULL);
    vkFreeMemory(device, depthImageMemory, NULL);
    for(auto framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer, NULL);
    vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
    vkDestroyPipeline(device, graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    vkDestroyRenderPass(device, renderPass, NULL);
    for(auto imageView : swapChainImageViews)
        vkDestroyImageView(device, imageView, NULL);
    vkDestroySwapchainKHR(device, swapChain, NULL);
}

void VulkanInterface::cleanup()
{
    cleanupSwapChain();

    vkDestroySampler(device, textureSampler, NULL);
    vkDestroyImageView(device, textureImageView, NULL);
    vkDestroyImage(device, textureImage, NULL);
    vkFreeMemory(device, textureImageMemory, NULL);
    vkDestroyDescriptorPool(device, descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
    vkDestroyBuffer(device, uniformBuffer, NULL);
    vkFreeMemory(device, uniformBufferMemory, NULL);
    vkDestroyBuffer(device, combinedBuffer, NULL);
    vkFreeMemory(device, combinedBufferMemory, NULL);
    vkDestroySemaphore(device, renderFinishedSemaphore, NULL);
    vkDestroySemaphore(device, imageAvailableSemaphore, NULL);
    for(auto fence : fences)
        vkDestroyFence(device, fence, NULL);
    vkDestroyCommandPool(device, commandPool, NULL);
    vkDestroyDevice(device, NULL);
#ifdef ENABLE_VALIDATION_LAYERS
    destroyDebugReportCallbackEXT(instance, callback, NULL);
#endif
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
}

void VulkanInterface::createInstance()
{
#ifdef ENABLE_VALIDATION_LAYERS
    if(!checkValidationLayerSupport())
    {
        LOG(ERR) << "No validation layers supported";
        assert(false);
    }
#endif

    //Create application info struct
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = APPLICATION_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, POINT_VERSION);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, POINT_VERSION);
    appInfo.apiVersion = VULKAN_API_VERSION;

    //Get required extensions for SDL (https://gist.github.com/rcgordon/ad23f873393423e1f1069502b92ad035)
    unsigned int count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &count, NULL);
    const char **names = new const char *[count];
    SDL_Vulkan_GetInstanceExtensions(window, &count, names);
    std::vector<const char*> requiredExtensions(names, names + count);
#ifdef ENABLE_VALIDATION_LAYERS
    requiredExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    //Optional: Show required extensions
    LOG(INFO) << "Required Extensions:";
    for(auto extension : requiredExtensions)
        LOG(INFO) << '\t' << extension;

    //Optional: Show available extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions.data());
    LOG(INFO) << "Available extensions:";
    for(const auto& extension : extensions)
        LOG(INFO) << '\t' << extension.extensionName;

    //Create application instance info struct
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
#ifdef ENABLE_VALIDATION_LAYERS
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif
    if(vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS)
    {
        LOG(ERR) << "Error creating Vulkan instance";
        assert(false);
    }

    //Cleanup allocated memory
    delete[] names;
}

#ifdef ENABLE_VALIDATION_LAYERS
bool VulkanInterface::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;
    }

    return true;
}
#endif
