#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <string>
#include <array>
#include "RenderState.h"

class ResourceLoader;

#ifdef _DEBUG
#define ENABLE_VALIDATION_LAYERS

struct DbgVertex
{
    glm::vec3 pos;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(DbgVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(DbgVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(DbgVertex, color);

        return attributeDescriptions;
    }
};

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

struct Vertex
{
    glm::vec3 pos;
    glm::vec4 color;
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
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
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
#ifdef _DEBUG
    VkPipeline debugGeometryGraphicsPipeline;
    VkPipeline debugOutlineGraphicsPipeline;
    VkPipeline debugPointGraphicsPipeline;
    VkDeviceSize dbgIndicesCount;
    VkDeviceSize dbgVerticesCount;
#endif
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    std::vector<VkFence> fences;
    std::vector<bool> bufferSubmitted;
    VkBuffer combinedBuffer;
    VkDeviceMemory combinedBufferMemory;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
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

    ResourceLoader* m_resourceLoader;

    //No default constructor
    VulkanInterface() {};

public:
    VulkanInterface(SDL_Window* window, ResourceLoader* shaderLoader);  //Resource loader to handle loading shaders
    ~VulkanInterface();

    void mainLoop(const RenderState& state);
    void resizeWindow();

#ifdef _DEBUG
    //For debugging stuff
    std::vector<DbgVertex> dbgPolyVertices;
    std::vector<uint16_t> dbgPolyIndices;
    std::vector<uint16_t> dbgPolyLines;
    uint32_t polyLineIdx;
    uint32_t polyPointIdx;
#endif

private:
    //Private member functions
#ifdef ENABLE_VALIDATION_LAYERS
    void setupDebugCallback();
    bool checkValidationLayerSupport();
#endif

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
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createSemaphores();
    void createFences();
    void createCommandBuffers();
    void setupCommandBuffer(uint32_t index);
    void createCommandPool();
    void createFramebuffers();
    void createRenderPass();
    void createGraphicsPipelines();
    VkShaderModule createShaderModule(const std::string& filename);
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
    void cleanupVertBufferMemory();
    void cleanup();
    void createInstance();

};
