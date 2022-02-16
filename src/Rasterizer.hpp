#pragma once

#include "Context.hpp"
#include <vector>

class Rasterizer final
{
public:
    Rasterizer(const Context& context);
    ~Rasterizer();

private:
    void createRenderPass();
    void createDepthImage();
    void createImageViews();
    void createFramebuffers();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();

    const Context& m_context;
    VkDevice m_device;

    VkRenderPass m_renderPass;
    VkImage m_depthImage;
    VkDeviceMemory m_depthImageMemory;
    std::vector<VkImageView> m_swapchainImageViews;
    VkImageView m_depthImageView;
    std::vector<VkFramebuffer> m_framebuffers;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
};
