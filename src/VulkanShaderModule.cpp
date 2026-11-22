
#include "VulkanShaderModule.h"

#include <fstream>

#include "VulkanConfig.h"
#include "VulkanDevice.h"


VulkanShaderModule::VulkanShaderModule(const std::string& filepath, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    compileShaderAtRuntime_(visualizerConfig.shaderModuleConfig_.compileShaderAtRuntime_),
    compileShaderWithDebugFlags_(visualizerConfig.shaderModuleConfig_.compileShaderWithDebugFlags_),
    minimumVulkanVersion_(visualizerConfig.shaderModuleConfig_.minimumVulkanVersion_)
{
    LOG_DBG(this->logger_) << "Creating shader module. "
        << "filepath <" << filepath << ">, "
        << "compileShaderAtRuntime <" << std::format("{}", this->compileShaderAtRuntime_) << ">, "
        << "compileShaderWithDebugFlags <" << std::format("{}", this->compileShaderWithDebugFlags_) << ">, "
        << "minimumVulkanVersion <" << this->ApiVersionToString(this->minimumVulkanVersion_) << ">." << std::endl;

    this->CreateShaderModule(filepath, device);

    LOG_DBG(this->logger_) << "Shader module created." << std::endl;
}

VulkanShaderModule::~VulkanShaderModule()
{
    LOG_DBG(this->logger_) << "Shader module destroyed. " << std::endl;
}

vk::ShaderModule VulkanShaderModule::Get() const
{
    return this->shaderModule_.get();
}

std::string VulkanShaderModule::ApiVersionToString(uint32_t apiVersion) const
{
    return std::to_string(vk::apiVersionMajor(apiVersion)) + "." + std::to_string(vk::apiVersionMinor(apiVersion)) + "." + std::to_string(vk::apiVersionPatch(apiVersion));
}

std::string VulkanShaderModule::LoadGLSLFile(const std::string& filepath) const
{
    std::string fileContent;

    // File is loaded as a binary file from the end.
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (file.is_open() == false)
    {
        throw std::runtime_error(std::format("Exception: File with filepath <{}> could not be opened.", filepath));
    }

    // Position indicator points to the end of the file, so its value is equal to the file size.
    size_t fileSize = static_cast<size_t>(file.tellg());
    fileContent.resize(fileSize);

    // The file data are read from the beginning.
    file.seekg(0);
    file.read(fileContent.data(), fileSize);

    file.close();

    return fileContent;
}

std::vector<uint32_t> VulkanShaderModule::CompileGLSLShader(const std::string& shaderSource, const std::string& shaderFilepath) const
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    switch (vk::apiVersionMinor(this->minimumVulkanVersion_))
    {
        case 0:
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
            break;
        case 1:
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
            break;
        case 2:
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
            break;
        case 3:
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
            break;
        case 4:
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
            break;
        default:
            LOG_WRN(this->logger_) << "Target environment for a minor version <" << vk::apiVersionMinor(this->minimumVulkanVersion_) << "> is not supported." << std::endl;
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
    }

    if (this->compileShaderWithDebugFlags_ == true)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_zero);
        options.SetGenerateDebugInfo();
        options.SetWarningsAsErrors();
    }
    else
    {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    // shaderc_glsl_infer_from_source infers shader type based on the #pragma in the source file.
    // Examples can be found in these tests: https://github.com/google/shaderc/blob/4dc596ddc2702092c670e828745dc3e0338d83c1/libshaderc/src/common_shaders_for_test.h
    // #pragma shader_stage(vertex)
    // #pragma shader_stage(fragment)
    // ....
    shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(shaderSource, shaderc_glsl_infer_from_source, shaderFilepath.c_str(), options);

    if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        throw std::runtime_error("Exception: Error during compilation of a shader file with filepath < " + shaderFilepath +  ">. Error message is <" + compilationResult.GetErrorMessage() + ">.");
    }

    return std::vector<uint32_t>(compilationResult.begin(), compilationResult.end());
}

void VulkanShaderModule::CreateShaderModule(const std::string& filepath, const VulkanDevice& device)
{
    std::vector<uint32_t> spirV;

    if (this->compileShaderAtRuntime_ == true)
    {
        spirV = this->CompileGLSLShader(this->LoadGLSLFile(filepath), filepath);
    }
    else
    {
        throw std::runtime_error("Exception: Loading of precompiled shaders is not implemented.");
    }

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};

    shaderModuleCreateInfo
        .setCode(spirV);

    this->shaderModule_ = device.Get().createShaderModuleUnique(shaderModuleCreateInfo);
}
