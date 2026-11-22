
#include "VulkanFrame.h"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "VulkanConfig.h"
#include "VulkanInternalConfig.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanHelperEnums.h"
#include "VulkanHelperFunctions.h"
#include "VulkanHelperStructs.h"
#include "VulkanGlobalData.h"
#include "VulkanDescriptorSetLayout.h"


VulkanGraphicsFrameContext::VulkanGraphicsFrameContext(const VulkanDevice& device, const VulkanSwapchain& swapchain, const VulkanVMA& vma, const VulkanDescriptorPool& descriptorPool, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    device_(device),
    vma_(vma),
    // Command pool and its buffer will be reset often (each frame).
    commandPool_(device, device.GetGraphicsQueueFamilyIndex(), vk::CommandPoolCreateFlagBits::eTransient, visualizerConfig),
    vertexBuffer_(vma, this->SetupVertexBufferProperties(visualizerConfig), visualizerConfig),
    indexBuffer_(vma, this->SetupIndexBufferProperties(visualizerConfig), visualizerConfig),
    uniformBuffer_(vma, this->SetupUniformBufferProperties(), visualizerConfig),
    depthImage_(std::make_unique<VulkanImage>(device, vma, this->SetupDepthImageAndViewsProperties(swapchain, internalConfig, visualizerConfig), visualizerConfig)),
    msaaImage_(std::make_unique<VulkanImage>(device, vma, this->SetupMSAAImageAndViewsProperties(swapchain, visualizerConfig), visualizerConfig)),
    globalDataDescriptorSet_(device, descriptorPool, descriptorSetLayouts[0].get(), visualizerConfig)
{
    LOG_DBG(this->logger_) << "Creating graphics frame context." << std::endl;

    this->descriptorSetsHandles_.insert(this->descriptorSetsHandles_.end(), {globalDataDescriptorSet_.Get()});

    this->UpdateDescriptorSets(device);

    LOG_DBG(this->logger_) << "Graphics frame context created." << std::endl;
}

VulkanGraphicsFrameContext::~VulkanGraphicsFrameContext()
{
    LOG_DBG(this->logger_) << "Graphics frame context destroyed. " << std::endl;
}

VulkanCommandPool& VulkanGraphicsFrameContext::GetCommandPool()
{
    return this->commandPool_;
}

VulkanBuffer& VulkanGraphicsFrameContext::GetVertexBuffer()
{
    return this->vertexBuffer_;
}

VulkanBuffer& VulkanGraphicsFrameContext::GetIndexBuffer()
{
    return this->indexBuffer_;
}

VulkanBuffer& VulkanGraphicsFrameContext::GetUniformBuffer()
{
    return this->uniformBuffer_;
}

VulkanImage& VulkanGraphicsFrameContext::GetDepthImage()
{
    return *this->depthImage_;
}

void VulkanGraphicsFrameContext::RecreateDepthImage(const VulkanSwapchain& swapchain, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig)
{
    LOG_DBG(this->logger_) << "Recreating depth image. " << std::endl;
    
    this->depthImage_ = std::make_unique<VulkanImage>(this->device_.get(), this->vma_, this->SetupDepthImageAndViewsProperties(swapchain, internalConfig, visualizerConfig), visualizerConfig);

    LOG_DBG(this->logger_) << "Depth image recreated." << std::endl;
}

VulkanImage& VulkanGraphicsFrameContext::GetMSAAImage()
{
    return *this->msaaImage_;
}

void VulkanGraphicsFrameContext::RecreateMSAAImage(const VulkanSwapchain& swapchain, const VulkanVisualizerConfig& visualizerConfig)
{
    LOG_DBG(this->logger_) << "Recreating MSAA image. " << std::endl;

    this->msaaImage_ = std::make_unique<VulkanImage>(this->device_.get(), this->vma_, this->SetupMSAAImageAndViewsProperties(swapchain, visualizerConfig), visualizerConfig);

    LOG_DBG(this->logger_) << "MSAA image recreated." << std::endl;
}

const std::vector<vk::DescriptorSet>& VulkanGraphicsFrameContext::GetDescriptorSetsHandles() const
{
    return this->descriptorSetsHandles_;
}

void VulkanGraphicsFrameContext::UpdateDescriptorSets(const VulkanDevice& device) const
{
    // Connects descriptor sets with actual source resources.

    // Global data descriptor.
    vk::DescriptorBufferInfo globalDataDescriptorBufferInfo{};

    globalDataDescriptorBufferInfo
        .setBuffer(this->uniformBuffer_.Get())
        .setOffset(0)
        .setRange(sizeof(VulkanGlobalData));

    vk::WriteDescriptorSet globalDataWriteDescriptorSet{};

    globalDataWriteDescriptorSet
        .setDstSet(this->globalDataDescriptorSet_.Get())
        .setDstBinding(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo(globalDataDescriptorBufferInfo);

    const std::array<vk::WriteDescriptorSet, 1> writeDescriptorSets = {globalDataWriteDescriptorSet};

    device.Get().updateDescriptorSets(writeDescriptorSets, {});
}

VulkanBufferProperties VulkanGraphicsFrameContext::SetupVertexBufferProperties(const VulkanVisualizerConfig& visualizerConfig) const
{
    VulkanBufferUseCase bufferUseCase = VulkanBufferUseCase::Vertex;

    VulkanBufferProperties bufferProperties(
        bufferUseCase,
        visualizerConfig.bufferConfig_.bufferSizes_.at(bufferUseCase),
        // Transfer destination for staging buffer, vertex buffer.
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        // Device memory.
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        // No access flags.
        0,
        // One queue owns buffer at a time.
        vk::SharingMode::eExclusive);

    return bufferProperties;
}

VulkanBufferProperties VulkanGraphicsFrameContext::SetupIndexBufferProperties(const VulkanVisualizerConfig& visualizerConfig) const
{
    VulkanBufferUseCase bufferUseCase = VulkanBufferUseCase::Index;

    VulkanBufferProperties bufferProperties(
        bufferUseCase,
        visualizerConfig.bufferConfig_.bufferSizes_.at(bufferUseCase),
        // Transfer destination for staging buffer, vertex buffer.
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        // Device memory.
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        // No access flags.
        0,
        // One queue owns buffer at a time.
        vk::SharingMode::eExclusive);

    return bufferProperties;
}

VulkanBufferProperties VulkanGraphicsFrameContext::SetupUniformBufferProperties() const
{
    VulkanBufferUseCase bufferUseCase = VulkanBufferUseCase::Uniform;

    VulkanBufferProperties bufferProperties(
        bufferUseCase,
        sizeof(VulkanGlobalData),
        // Uniform buffer.
        vk::BufferUsageFlagBits::eUniformBuffer,
        // Host memory.
        VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
        // Sequential write, permanently mapped.
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        // One queue owns buffer at a time.
        vk::SharingMode::eExclusive);

    return bufferProperties;
}

std::pair<VulkanImageProperties, std::array<VulkanImageViewProperties, 1>> VulkanGraphicsFrameContext::SetupDepthImageAndViewsProperties(const VulkanSwapchain& swapchain, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig) const
{
    VulkanImageUseCase imageUseCase = VulkanImageUseCase::Depth;

    VulkanImageProperties imageProperties(
        imageUseCase,
        internalConfig.depthFormat_,
        // Depth extent matches that of a swapchain.
        vk::Extent3D{swapchain.GetExtent().width, swapchain.GetExtent().height, 1},
        // Depth attachment.
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        // One queue owns the image buffer at a time.
        vk::SharingMode::eExclusive,
        // One mip level.
        1,
        // One array layer.
        1,
        // Chosen MSAA sample count.
        visualizerConfig.imageConfig_.imageSampleCounts_.at(imageUseCase));

    // Single image view.
    VulkanImageViewProperties imageViewProperties(
        // Depth image view
        vk::ImageAspectFlagBits::eDepth,
        internalConfig.depthFormat_);

    return {imageProperties, {imageViewProperties}};
}

std::pair<VulkanImageProperties, std::array<VulkanImageViewProperties, 1>> VulkanGraphicsFrameContext::SetupMSAAImageAndViewsProperties(const VulkanSwapchain& swapchain, const VulkanVisualizerConfig& visualizerConfig) const
{
    VulkanImageUseCase imageUseCase = VulkanImageUseCase::MSAA;

    VulkanImageProperties imageProperties(
        imageUseCase,
        swapchain.GetSurfaceFormat().format,
        // MSAA color extent matches the swapchain's extent.
        vk::Extent3D{swapchain.GetExtent().width, swapchain.GetExtent().height, 1},
        // Used as a color attachment.
        vk::ImageUsageFlagBits::eColorAttachment,
        // One queue owns the image at a time.
        vk::SharingMode::eExclusive,
        // One mip level.
        1,
        // One array layer.
        1,
        // Chosen MSAA sample count.
        visualizerConfig.imageConfig_.imageSampleCounts_.at(imageUseCase));

    VulkanImageViewProperties imageViewProperties(
        // Color image view.
        vk::ImageAspectFlagBits::eColor,
        swapchain.GetSurfaceFormat().format);

    return {imageProperties, {imageViewProperties}};
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanTransferFrameContext::VulkanTransferFrameContext(const VulkanDevice& device, const VulkanVMA& vma, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    // Command pool and its buffer will be reset often (each frame).
    commandPool_(device, device.GetTransferQueueFamilyIndex(), vk::CommandPoolCreateFlagBits::eTransient, visualizerConfig),
    stagingBuffer_(vma, this->SetupStagingBufferProperties(visualizerConfig), visualizerConfig)
{
    LOG_DBG(this->logger_) << "Creating transfer frame context. " << std::endl;

    LOG_DBG(this->logger_) << "Transfer frame context created." << std::endl;
}

VulkanTransferFrameContext::~VulkanTransferFrameContext()
{
    LOG_DBG(this->logger_) << "Transfer frame context destroyed. " << std::endl;
}

VulkanCommandPool& VulkanTransferFrameContext::GetCommandPool()
{
    return this->commandPool_;
}

VulkanBuffer& VulkanTransferFrameContext::GetStagingBuffer()
{
    return this->stagingBuffer_;
}

VulkanBufferProperties VulkanTransferFrameContext::SetupStagingBufferProperties(const VulkanVisualizerConfig& visualizerConfig) const
{
    VulkanBufferUseCase bufferUseCase = VulkanBufferUseCase::Staging;

    VulkanBufferProperties bufferProperties(
        bufferUseCase,
        visualizerConfig.bufferConfig_.bufferSizes_.at(bufferUseCase),
        // Transfer source for vertex and index buffer.
        vk::BufferUsageFlagBits::eTransferSrc,
        // Host accessible memory.
        VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
        // Sequential write, permanently mapped.
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        // One queue owns buffer at a time.
        vk::SharingMode::eExclusive);

    return bufferProperties;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanFrameContext::VulkanFrameContext(const VulkanDevice& device, const VulkanSwapchain& swapchain, const VulkanVMA& vma, const VulkanDescriptorPool& descriptorPool, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    graphicsFrameContext_(device, swapchain, vma, descriptorPool, descriptorSetLayouts, internalConfig, visualizerConfig),
    transferFrameContext_(device, vma, visualizerConfig)
{
    LOG_DBG(this->logger_) << "Creating frame context. " << std::endl;

    // Synchronization primitives are created.
    this->CreateFences(device);
    this->CreateSemaphores(device);

    LOG_DBG(this->logger_) << "Frame context created." << std::endl;
}

VulkanFrameContext::~VulkanFrameContext()
{
    LOG_DBG(this->logger_) << "Frame context destroyed. " << std::endl;
}

void VulkanFrameContext::FirstFrameHandled()
{
    this->firstFrame_ = false;
}

bool VulkanFrameContext::HandlingFirstFrame() const
{
    return this->firstFrame_;
}

VulkanGraphicsFrameContext& VulkanFrameContext::GetGraphicsFrameContext()
{
    return this->graphicsFrameContext_;
}

VulkanTransferFrameContext& VulkanFrameContext::GetTransferFrameContext()
{
    return this->transferFrameContext_;
}

vk::Fence VulkanFrameContext::GetFrameInFlightFence() const
{
    return this->frameInFlightFence_.get();
}

vk::Semaphore VulkanFrameContext::GetImageAvailableSemaphore() const
{
    return this->imageAvailableSemaphore_.get();
}

vk::Semaphore VulkanFrameContext::GetTransferFinishedSemaphore() const
{
    return this->transferFinishedSemaphore_.get();
}

vk::Semaphore VulkanFrameContext::GetGraphicsFinishedSemaphore() const
{
    return this->graphicsFinishedSemaphore_.get();
}

void VulkanFrameContext::CreateFences(const VulkanDevice& device)
{
    vk::FenceCreateInfo frameInFlightFenceCreateInfo{};

    frameInFlightFenceCreateInfo
        // Frame in flight fence starts in a signaled state which allows the first frame to be processed.
        .setFlags(vk::FenceCreateFlagBits::eSignaled);

    this->frameInFlightFence_ = device.Get().createFenceUnique(frameInFlightFenceCreateInfo);
}

void VulkanFrameContext::CreateSemaphores(const VulkanDevice& device)
{
    vk::SemaphoreCreateInfo imageAvailableSemaphoreCreateInfo{};

    this->imageAvailableSemaphore_ = device.Get().createSemaphoreUnique(imageAvailableSemaphoreCreateInfo);


    vk::SemaphoreCreateInfo transferFinishedSemaphoreCreateInfo{};

    this->transferFinishedSemaphore_ = device.Get().createSemaphoreUnique(transferFinishedSemaphoreCreateInfo);


    vk::SemaphoreCreateInfo graphicsFinishedSemaphoreCreateInfo{};

    this->graphicsFinishedSemaphore_ = device.Get().createSemaphoreUnique(graphicsFinishedSemaphoreCreateInfo);
}
