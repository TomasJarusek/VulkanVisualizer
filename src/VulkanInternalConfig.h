
#pragma once


struct VulkanInternalConfig
{
    // 2D/3D mode.
    bool is2D_ = false;

    // Maximum number of vertices that can be drawn every frame.
    size_t maxVertexCount_ = 1000000;

    // Predefined world axes.
    glm::vec3 right_ = {1.0f, 0.0f, 0.0f};
    glm::vec3 forward_ = {0.0f, 1.0f, 0.0f};
    glm::vec3 up_ = {0.0f, 0.0f, 1.0f};

    // Depth.
    std::array<vk::Format, 3> acceptableDepthFormats_ = {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint};
    vk::Format depthFormat_{};

    size_t framesInFlightCount_ = 2;
    size_t currentFrameInFlightIndex_ = 0;

    bool useIndexedDraw_ = true;

    bool useMSAA_ = false;

    glm::vec4 backgroundColor_ = {0.0f, 0.0f, 0.0f, 1.0f};

    glm::vec4 lightSourcePosition_ = {0.0f, 0.0f, 100.0f, 0.0f};
    glm::vec4 lightSourceColor_ = {1.0f, 1.0f, 1.0f, 0.0f};

    glm::vec4 phongParameters_ = {0.0f, 0.15f, 64.0f, 0.0f};

    bool showLiveTimingInfo_ = false;
    bool showLiveFrameInfo_ = false;
    bool showPostCloseTimingInfo_ = false;
};
