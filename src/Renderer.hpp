#pragma once

#include "Context.hpp"
#include "Camera.hpp"
#include <vector>

class Renderer final
{
public:
    Renderer(Context& context);
    ~Renderer();

    bool render();

private:
    void setupCamera();
    void createRenderPass();
    void createDepthImage();
    void createImageViews();
    void createFramebuffers();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createDescriptorPool();
    void createDescriptorSet();
    void createUniformBuffer();
    void updateDescriptorSet();
    void createVertexAndIndexBuffer();
    void allocateCommandBuffers();

    Context& m_context;
    VkDevice m_device;

    Camera m_camera;
    VkRenderPass m_renderPass;
    VkImage m_depthImage;
    VkDeviceMemory m_depthImageMemory;
    std::vector<VkImageView> m_swapchainImageViews;
    VkImageView m_depthImageView;
    std::vector<VkFramebuffer> m_framebuffers;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSet;
    VkBuffer m_uniformBuffer;
    VkDeviceMemory m_uniformBufferMemory;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    size_t m_numIndices;
    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;
    std::vector<VkCommandBuffer> m_commandBuffers;
};
