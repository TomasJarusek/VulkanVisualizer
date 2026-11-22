
#pragma once


// Buffer use case.
enum class VulkanBufferUseCase {Undefined, Staging, Vertex, Index, Uniform};

constexpr std::string_view ToString(VulkanBufferUseCase bufferUseCase)
{
    switch (bufferUseCase)
    {
        case VulkanBufferUseCase::Undefined:
            return "Undefined";
        case VulkanBufferUseCase::Staging:
            return "Staging";
        case VulkanBufferUseCase::Vertex:
            return "Vertex";
        case VulkanBufferUseCase::Index:
            return "Index";
        case VulkanBufferUseCase::Uniform:
            return "Uniform";
        default:
            return "Unknown";
    }
}

// Image use case.
enum class VulkanImageUseCase {Undefined, Depth, MSAA};

constexpr std::string_view ToString(VulkanImageUseCase imageUseCase)
{
    switch (imageUseCase)
    {
        case VulkanImageUseCase::Undefined:
            return "Undefined";
        case VulkanImageUseCase::Depth:
            return "Depth";
        case VulkanImageUseCase::MSAA:
            return "MSAA";
        default:
            return "Unknown";
    }
}

// Pipeline use case.
enum class VulkanPipelineUseCase {Undefined, Opaque, Transparent};

constexpr std::string_view ToString(VulkanPipelineUseCase pipelineUseCase)
{
    switch (pipelineUseCase)
    {
        case VulkanPipelineUseCase::Undefined:
            return "Undefined";
        case VulkanPipelineUseCase::Opaque:
            return "Opaque";
        case VulkanPipelineUseCase::Transparent:
            return "Transparent";
        default:
            return "Unknown";
    }
}

// Window mode.
enum class VulkanWindowMode {Window, BorderlessFullscreen, ExclusiveFullscreen};

constexpr std::string_view ToString(VulkanWindowMode pipelineUseCase)
{
    switch (pipelineUseCase)
    {
    case VulkanWindowMode::Window:
        return "Window";
    case VulkanWindowMode::BorderlessFullscreen:
        return "Borderless fullscreen";
    case VulkanWindowMode::ExclusiveFullscreen:
        return "Exclusive fullscreen ";
    default:
        return "Unknown";
    }
}
