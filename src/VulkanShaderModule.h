
#pragma once

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <string>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanDevice;


class VulkanShaderModule
{
private:

    Logger logger_;

    bool compileShaderAtRuntime_ = false;
    bool compileShaderWithDebugFlags_ = false;
    uint32_t minimumVulkanVersion_ = vk::makeVersion(1, 0, 0);

    vk::UniqueShaderModule shaderModule_;

public:

    // Constructor.
    VulkanShaderModule(const std::string& filepath, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanShaderModule();

    // Copy/move constructor/assignment.
    VulkanShaderModule(const VulkanShaderModule& shaderModule) = delete;
    VulkanShaderModule(VulkanShaderModule&& shaderModule) noexcept = delete;
    VulkanShaderModule& operator=(const VulkanShaderModule& shaderModule) = delete;
    VulkanShaderModule& operator=(VulkanShaderModule& shaderModule) noexcept = delete;

    vk::ShaderModule Get() const;

private:

    std::string ApiVersionToString(uint32_t apiVersion) const;

    std::string LoadGLSLFile(const std::string& filepath) const;
    std::vector<uint32_t> CompileGLSLShader(const std::string& shaderSource, const std::string& shaderFilepath) const;

    void CreateShaderModule(const std::string& filepath, const VulkanDevice& device);
};
