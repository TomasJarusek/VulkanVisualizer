
#include "VulkanHelperFunctions.h"

#include "VulkanHelperStructs.h"


vk::ImageMemoryBarrier2 SetupImageMemoryBarrier(vk::Image image, const VulkanImageState& srcImageState, const VulkanImageState& dstImageState, const vk::ImageSubresourceRange& subresourceRange)
{
	vk::ImageMemoryBarrier2 imageMemoryBarrier{};

	imageMemoryBarrier
		.setImage(image);

	// Stage, access and layout.
	imageMemoryBarrier
		.setSrcStageMask(srcImageState.stage_)
		.setSrcAccessMask(srcImageState.access_)
		.setOldLayout(srcImageState.layout_)
		.setDstStageMask(dstImageState.stage_)
		.setDstAccessMask(dstImageState.access_)
		.setNewLayout(dstImageState.layout_);

	// Queue family.
	imageMemoryBarrier
		.setSrcQueueFamilyIndex(srcImageState.queueFamilyIndex_)
		.setDstQueueFamilyIndex(dstImageState.queueFamilyIndex_);

	// Subresource range is provided separately.
	imageMemoryBarrier
		.setSubresourceRange(subresourceRange);

	return imageMemoryBarrier;
}

vk::BufferMemoryBarrier2 SetupBufferMemoryBarrier(vk::Buffer buffer, const VulkanBufferState& srcBufferState, const VulkanBufferState& dstBufferState, vk::DeviceSize size, vk::DeviceSize offset)
{
	vk::BufferMemoryBarrier2 bufferMemoryBarrier{};

	bufferMemoryBarrier
		.setBuffer(buffer);

	// Stage and access.
	bufferMemoryBarrier
		.setSrcStageMask(srcBufferState.stage_)
		.setSrcAccessMask(srcBufferState.access_)
		.setDstStageMask(dstBufferState.stage_)
		.setDstAccessMask(dstBufferState.access_);

	// Queue family.
	bufferMemoryBarrier
		.setSrcQueueFamilyIndex(srcBufferState.queueFamilyIndex_)
		.setDstQueueFamilyIndex(dstBufferState.queueFamilyIndex_);

	// Size and offset are provided separately.
	bufferMemoryBarrier
		.setSize(size)
		.setOffset(offset);

	return bufferMemoryBarrier;
}
