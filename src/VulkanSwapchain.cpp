
#include "VulkanSwapchain.h"

#include "VulkanConfig.h"
#include "VulkanWindow.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanHelperStructs.h"
#include "VulkanHelperFunctions.h"


VulkanSwapchain::VulkanSwapchain(const VulkanWindow& window, const VulkanSurface& surface, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    device_(device)
{
    LOG_DBG(this->logger_) << "Creating swapchain." << std::endl;

    // Specific option is chosen for each swapchain setting.
    this->ChooseSurfaceFormat(device);
    this->ChoosePresentMode(device);
    this->ChooseExtent(window, device);

    // Swapchain is created.
    this->CreateSwapchain(surface, device);

    // Swapchain image views are created.
    this->CreateSwapchainImageViews(device);

    // Render finished semaphores are created.
    this->CreateRenderFinishedSemaphores(device);

    LOG_DBG(this->logger_) << "Swapchain created." << std::endl;
}

VulkanSwapchain::VulkanSwapchain(const VulkanWindow& window, const VulkanSurface& surface, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig, const VulkanSwapchain& oldSwapchain) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    device_(device)
{
    LOG_DBG(this->logger_) << "Recreating swapchain. " << std::endl;

    // During recreation, the present mode and extent are chosen
    // again the same way as during the initial creation.
    this->ChoosePresentMode(device);
    this->ChooseExtent(window, device);

    // Surface format is different. If the surface format changes, the pipeline 
    // would have to be recreated as well, which technically is doable, but that
    // complicates things, the recreation would be slow and it is unlikely to happen.
    // The better and optimal solution would be draw into an off screen buffer first
    // and then transition the off screen buffer to a proper format before writing its
    // contents to the swapchain image. That is unnecessarily complicated for now.
    // So instead, we will simply check if the existing format is still available.
    // If not, then the application cannot continue with the execution.
    // Standard SDR formats will probably always be supported anyway, so this can only
    // realistically happen when a window is moved from a HDR to a SDR monitor.
    if (this->IsSurfaceFormatSupported(device, oldSwapchain.GetSurfaceFormat()) == false)
    {
        throw std::runtime_error("Exception: Surface format that was used before recreation is no longer available.");
    }

    // Old swapchain handle is provided to the new swapchain construction.
    this->CreateSwapchain(surface, device, oldSwapchain.Get());

    // Swapchain image views are created.
    this->CreateSwapchainImageViews(device);

    // Render finished semaphores are created.
    this->CreateRenderFinishedSemaphores(device);

    LOG_DBG(this->logger_) << "Swapchain recreated." << std::endl;
}

VulkanSwapchain::~VulkanSwapchain()
{
    LOG_DBG(this->logger_) << "Swapchain destroyed. " << std::endl;
}

vk::SwapchainKHR VulkanSwapchain::Get() const
{
    return this->swapchain_.get();
}

const vk::SurfaceFormatKHR& VulkanSwapchain::GetSurfaceFormat() const
{
    return this->surfaceFormat_;
}

const vk::Extent2D& VulkanSwapchain::GetExtent() const
{
    return this->extent_;
}

vk::ImageView VulkanSwapchain::GetSwapchainImageView(size_t swapchainImageIndex) const
{
    return this->swapchainImageViews_[swapchainImageIndex].get();
}

vk::Semaphore VulkanSwapchain::GetSwapchainImageRFSemaphore(size_t swapchainImageIndex) const
{
    return this->renderFinishedSemaphores_[swapchainImageIndex].get();
}

std::pair<vk::Result, uint32_t> VulkanSwapchain::AcquireNextImage(vk::Semaphore imageAvailableSemaphore) const
{
    uint32_t imageIndex = 0;

    // No timeout.
    // Once an image is available, the imageAvailableSemaphore is signaled.
    vk::Result acquireNextImageResult = this->device_.get().Get().acquireNextImageKHR(this->swapchain_.get(), std::numeric_limits<std::uint64_t>::max(), imageAvailableSemaphore, {}, &imageIndex);

    return std::make_pair(acquireNextImageResult, imageIndex);
}

vk::Result VulkanSwapchain::PresentImage(size_t swapchainImageIndex) const
{
    std::array<uint32_t, 1> swapchainImageIndexes = {{static_cast<uint32_t>(swapchainImageIndex)}};

    vk::PresentInfoKHR presentInfo{};

    presentInfo
        .setWaitSemaphores(this->renderFinishedSemaphores_[swapchainImageIndex].get())
        .setSwapchains(this->swapchain_.get())
        .setImageIndices(swapchainImageIndexes);

    // Always a presentation queue.
    return this->device_.get().GetPresentationQueue().presentKHR(presentInfo);
}

void VulkanSwapchain::InsertSwapchainImageBarrier(vk::CommandBuffer commandBuffer, size_t swapchainImageIndex, const VulkanImageState& srcImageState, const VulkanImageState& dstImageState) const
{
    vk::ImageSubresourceRange imageSubresourceRange{};

    imageSubresourceRange
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    vk::ImageMemoryBarrier2 imageMemoryBarrier = SetupImageMemoryBarrier(this->swapchainImages_[swapchainImageIndex], srcImageState, dstImageState, imageSubresourceRange);

    vk::DependencyInfo dependencyInfo{};

    dependencyInfo
        .setDependencyFlags({})
        .setImageMemoryBarriers(imageMemoryBarrier);

    commandBuffer.pipelineBarrier2(dependencyInfo);
}

float VulkanSwapchain::GetAspectRatio() const
{
    return static_cast<float>(this->extent_.width)/static_cast<float>(this->extent_.height);
}

void VulkanSwapchain::ChooseSurfaceFormat(const VulkanDevice& device)
{
    const std::vector<vk::SurfaceFormatKHR>& availableSurfaceFormats = device.GetPhysicalDeviceInfo().surfaceFormats_;
    // Only SDR formats are preferred. Because HDR can by definition support SDR (OS converts it),
    // this implies, that HDR can never be chosen with the current setup, which is fine for now.
    const std::array<std::pair<vk::Format, vk::ColorSpaceKHR>, 2> preferredSurfaceFormats = {{{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}, {vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}}};

    // Preferred surface formats are considered in order.
    for (const std::pair<vk::Format, vk::ColorSpaceKHR>& surfaceFormat : preferredSurfaceFormats)
    {
        // Each available surface format is checked for a match.
        auto surfaceFormatComparator = [surfaceFormat](const vk::SurfaceFormatKHR& surfaceFormatKHR)
        {
            return surfaceFormat.first == surfaceFormatKHR.format && surfaceFormat.second == surfaceFormatKHR.colorSpace;
        };

        auto foundSurfaceFormatIt = std::ranges::find_if(availableSurfaceFormats, surfaceFormatComparator);

        if (foundSurfaceFormatIt != availableSurfaceFormats.end())
        {
            // Preferred surface format is available.

            this->surfaceFormat_ = *foundSurfaceFormatIt;

            LOG_DBG(this->logger_) << "Preferred surface format <" << GetPrintableValue(this->surfaceFormat_.format) << ", " << GetPrintableValue(this->surfaceFormat_.colorSpace) << "> was chosen." << std::endl;

            return;
        }
    }

    // No preferred surface format is available. First one is taken as a fallback.
    // Surface formats vector cannot be empty, otherwise it would be rejected during a physical device pick.
    this->surfaceFormat_ = availableSurfaceFormats.front();

    LOG_DBG(this->logger_) << "Fallback surface format <" << GetPrintableValue(this->surfaceFormat_.format) << ", " << GetPrintableValue(this->surfaceFormat_.colorSpace) << "> was chosen." << std::endl;
}

bool VulkanSwapchain::IsSurfaceFormatSupported(const VulkanDevice& device, const vk::SurfaceFormatKHR& surfaceFormatKHR)
{
    auto foundSurfaceFormatIt = std::ranges::find(device.GetPhysicalDeviceInfo().surfaceFormats_, surfaceFormatKHR);

    if (foundSurfaceFormatIt != device.GetPhysicalDeviceInfo().surfaceFormats_.end())
    {
        // Given format is available.

        this->surfaceFormat_ = *foundSurfaceFormatIt;

        LOG_DBG(this->logger_) << "Given surface format <" << GetPrintableValue(surfaceFormatKHR.format) << ", " << GetPrintableValue(surfaceFormatKHR.colorSpace) << "> is available." << std::endl;

        return true;
    }
    else
    {
        // Given format is not available.

        LOG_DBG(this->logger_) << "Given surface format <" << GetPrintableValue(surfaceFormatKHR.format) << ", " << GetPrintableValue(surfaceFormatKHR.colorSpace) << "> is not available." << std::endl;

        return false;
    }
}

void VulkanSwapchain::ChoosePresentMode(const VulkanDevice& device)
{
    const std::vector<vk::PresentModeKHR>& availablePresentModes = device.GetPhysicalDeviceInfo().presentModes_;
    const std::array<vk::PresentModeKHR, 2> preferredPresentModes = {{vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eFifo}};
    //const std::array<vk::PresentModeKHR, 1> preferredPresentModes = {{vk::PresentModeKHR::eFifo}};

    // Preferred present modes are considered in order.
    for (const vk::PresentModeKHR& presentMode : preferredPresentModes)
    {
        // Each available surface format is checked for a match.
        auto foundPresentModeIt = std::ranges::find(availablePresentModes, presentMode);

        if (foundPresentModeIt != availablePresentModes.end())
        {
            // Preferred present mode is available.

            this->presentMode_ = *foundPresentModeIt;

            LOG_DBG(this->logger_) << "Preferred present mode <" << GetPrintableValue(this->presentMode_) << "> was chosen." << std::endl;

            return;
        }
    }

    // FIFO is always guaranteed to be available.
    this->presentMode_ = vk::PresentModeKHR::eFifo;

    LOG_DBG(this->logger_) << "Fallback present mode <" << GetPrintableValue(this->presentMode_) << "> was chosen." << std::endl;
}

void VulkanSwapchain::ChooseExtent(const VulkanWindow& window, const VulkanDevice& device)
{
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities = device.GetPhysicalDeviceInfo().surfaceCapabilities_;

    // Vulkan tries to determine what resolution we should set for the swapchain images.
    // If the driver and the window system have negotiated successfully an exact height and width,
    // it is set by Vulkan and this value has to be used.
    // If this isn't the case, the Vulkan sets the variables to their maximum values and the resolution must be determined manually.
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        this->extent_ = surfaceCapabilities.currentExtent;

        LOG_DBG(this->logger_) << "Preset extent values were chosen. width <" << this->extent_.width << ">, height <" << this->extent_.height << ">" << std::endl;
    }
    else
    {
        // We start by querying the framebuffer size from GLFW.
        // The width and height values set in the initial window setup can't be used here because GLFW allows resolution and screen coordinates to be different.
        // The returned values represent the final resolution that the operating system will display.
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window.Get(), &width, &height);

        this->extent_.width = static_cast<uint32_t>(width);
        this->extent_.height = static_cast<uint32_t>(height);

        // Then another check has to be made. minImageExtent and maxImageExtent are values retrieved based on both the physical device and the surface.
        // So what can happen for example is that the OS can be capable of displaying 4k, but the surface/driver/display pipeline will clamp it to HD because they can't draw 4k.
        // That means that the swapchain has to work with HD images as well. The images will then be upscaled before they are drawn by the OS. 
        this->extent_.width = std::clamp(this->extent_.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        this->extent_.height = std::clamp(this->extent_.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

        LOG_DBG(this->logger_) << "Manually determined extent values were chosen. width <" << this->extent_.width << ">, height <" << this->extent_.height << ">" << std::endl;
    }
}

void VulkanSwapchain::CreateSwapchain(const VulkanSurface& surface, const VulkanDevice& device, vk::SwapchainKHR oldSwapchainKHR)
{
    // Ideally, we want at least three images to have an option to set up triple buffering.
    // Minimum value must be respected.
    uint32_t minImageCount = std::max(3u, device.GetPhysicalDeviceInfo().surfaceCapabilities_.minImageCount);
    // Maximum value must be respected if set (not zero).
    if (device.GetPhysicalDeviceInfo().surfaceCapabilities_.maxImageCount > 0)
    {
        minImageCount = std::min(minImageCount, device.GetPhysicalDeviceInfo().surfaceCapabilities_.maxImageCount);
    }

    LOG_DBG(this->logger_) << "Swapchain minimum image count was set to <" << minImageCount << ">." << std::endl;


    vk::SwapchainCreateInfoKHR swapchainCreateInfo{};

    swapchainCreateInfo
        .setSurface(surface.Get())
        .setMinImageCount(minImageCount)
        .setImageFormat(this->surfaceFormat_.format)
        .setImageColorSpace(this->surfaceFormat_.colorSpace)
        .setImageExtent(this->extent_)
        // Always one unless used in a stereoscopic application.
        .setImageArrayLayers(1)
        // Rendering will be done directly into a swapchain image, so a color attachment is chosen.
        // If for example we would render into a different image first, do post-processing
        // and then transfer the result into a swapchain image, a vk::ImageUsageFlagBits::eTransferDst would be used instead.
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        // Stuff like 90 degree rotation can be applied to the images.
        // We don't want any, so the current transform is assigned.
        .setPreTransform(device.GetPhysicalDeviceInfo().surfaceCapabilities_.currentTransform)
        // Specifies if alpha channel should be used when the final operating system windows
        // are composited over each other. Can be used for overlays for example.
        // vk::CompositeAlphaFlagBitsKHR::eOpaque means that the alpha channel will be ignored.
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(this->presentMode_)
        // Specifies if pixels hidden behind another window should be colored.
        .setClipped(vk::True);

    if (device.DoGraphicsAndPresentationQueuesShareQueueFamily() == true)
    {
        // Graphics and presentation share a queue family.
        // That means that both of them can access the resources without a transfer of ownership,
        // which allows us to use vk::SharingMode::eExclusive.

        swapchainCreateInfo
            .setImageSharingMode(vk::SharingMode::eExclusive);
    }
    else
    {
        // Graphics and presentation do not share a queue family.
        // For simplicity, a vk::SharingMode::eConcurrent will be used. Otherwise
        // a transfer of ownership of resources between queues would have to be handled manually.

        std::array<uint32_t, 2> queueFamilyIndexes = {{device.GetPhysicalDeviceInfo().graphicsQueueFamilyIndex_, device.GetPhysicalDeviceInfo().presentationQueueFamilyIndex_}};

        swapchainCreateInfo
            .setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(queueFamilyIndexes);    
    }

    // If this is called as a part of recreation, the old swapchain parameter will be filled. 
    if (oldSwapchainKHR != nullptr)
    {
        swapchainCreateInfo
            .setOldSwapchain(oldSwapchainKHR);
    }

    // Swapchain is created.
    this->swapchain_ = device.Get().createSwapchainKHRUnique(swapchainCreateInfo);

    // Swapchain images are retrieved.
    this->swapchainImages_ = device.Get().getSwapchainImagesKHR(this->swapchain_.get());
}

void VulkanSwapchain::CreateSwapchainImageViews(const VulkanDevice& device)
{
    // Defines which aspects of the image will be accessed through this view.
    vk::ImageSubresourceRange imageSubresourceRange{};

    imageSubresourceRange
        // Type of data.
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        // No mip mapping.
        .setBaseMipLevel(0)
        .setLevelCount(1)
        // Used for images with multiple layers (stereoscopic 3D, texture array).
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    // This specifies color mapping between view and the image itself.
    // For example, if the image would be greyscale, then we could
    // pipe that one value into all the RGB channels.
    // For now, the mapping is kept unchanged for all the channels.
    vk::ComponentMapping componentMapping{};

    componentMapping
        .setR(vk::ComponentSwizzle::eIdentity)
        .setG(vk::ComponentSwizzle::eIdentity)
        .setB(vk::ComponentSwizzle::eIdentity)
        .setA(vk::ComponentSwizzle::eIdentity);

    vk::ImageViewCreateInfo imageViewCreateInfo{};

    imageViewCreateInfo
        // Image will be interpreted as a 2D texture.
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(this->surfaceFormat_.format)
        .setSubresourceRange(imageSubresourceRange)
        .setComponents(componentMapping);

    // A view is created for each image.
    this->swapchainImageViews_.reserve(this->swapchainImages_.size());
    for (const vk::Image& image : this->swapchainImages_)
    {
        imageViewCreateInfo.setImage(image);

        this->swapchainImageViews_.push_back(device.Get().createImageViewUnique(imageViewCreateInfo));
    }
}

void VulkanSwapchain::CreateRenderFinishedSemaphores(const VulkanDevice& device)
{
    this->renderFinishedSemaphores_.reserve(this->swapchainImages_.size());
    for (size_t i = 0; i < this->swapchainImages_.size(); ++i)
    {
        vk::SemaphoreCreateInfo renderFinishedSemaphoreCreateInfo{};

        this->renderFinishedSemaphores_.push_back(device.Get().createSemaphoreUnique(renderFinishedSemaphoreCreateInfo));
    }
}
