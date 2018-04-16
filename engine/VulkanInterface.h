#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <string>
#include "RenderState.h"

#ifdef _DEBUG
#define ENABLE_VALIDATION_LAYERS
#endif

struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete()
    {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanInterface
{
private:
    //Member variables
    SDL_Window* window;

#ifdef ENABLE_VALIDATION_LAYERS
    VkDebugReportCallbackEXT callback;
#endif
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;   //Implicitly destroyed when VkInstance is
    VkDevice device;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    std::vector<VkFence> fences;
    std::vector<bool> bufferSubmitted;
    VkBuffer combinedBuffer;
    VkDeviceMemory combinedBufferMemory;
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    uint32_t textureMipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    //No default constructor
    VulkanInterface() {};

public:
    VulkanInterface(SDL_Window* window);
    ~VulkanInterface();

    void mainLoop(const RenderState& state);
    void resizeWindow(int width, int height);

private:
    //Private member functions
#ifdef ENABLE_VALIDATION_LAYERS

    void setupDebugCallback();
#endif

    std::vector<char> readFile(const std::string& filename);

    void initVulkan();

    void createDepthResources();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat findDepthFormat();

    bool hasStencilComponent(VkFormat format);

    void createTextureSampler();

    void createTextureImageView();

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createTextureImage();

    void generateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void createDescriptorSet();

    void createDescriptorPool();

    void createUniformBuffer();

    void createDescriptorSetLayout();

    void createVertIndexBuffers();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createSemaphores();

    void createFences();

    void createCommandBuffers();

    void setupCommandBuffer(uint32_t index);

    void createCommandPool();

    void createFramebuffers();

    void createRenderPass();

    void createGraphicsPipeline();

    VkShaderModule createShaderModule(const std::vector<char>& code);

    void createSwapChainImageViews();

    void recreateSwapChain();

    void createSwapChain();

    void createSurface();

    void createLogicalDevice();

    void pickPhysicalDevice();

    int rateDeviceSuitability(VkPhysicalDevice device);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    void updateUniformBuffer(const RenderState& ubo);

    void drawFrame();

    void cleanupSwapChain();

    void cleanup();

    void createInstance();

#ifdef ENABLE_VALIDATION_LAYERS
    bool checkValidationLayerSupport();
#endif

};
