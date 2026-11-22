
#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanSwapchain;
class VulkanInputHandler;


class VulkanCamera
{
private:

    Logger logger_;

    // View matrix setup.
    // Chosen convention is:
    // +X -> east
    // +Y -> north
    // +Z -> up
    glm::vec3 position_;
    glm::vec3 targetPosition_;
    glm::vec3 up_;
    glm::vec3 upFallback_;

    // Perspective matrix setup.
    float fieldOfView_ = 0.0f;
    float aspectRatio_ = 0.0f;
    float nearPlane_ = 0.0f;
    float farPlane_ = 0.0f;

    // Orthographic matrix setup.
    uint32_t viewportWidth_ = 0;
    uint32_t viewportHeight_ = 0;

public:

    // Constructor.
    VulkanCamera(const VulkanSwapchain& swapchain, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanCamera();

    // Copy/move constructor/assignment.
    VulkanCamera(const VulkanCamera& swapchain) = delete;
    VulkanCamera(VulkanCamera&& swapchain) noexcept = delete;
    VulkanCamera& operator=(const VulkanCamera& swapchain) = delete;
    VulkanCamera& operator=(VulkanCamera& swapchain) noexcept = delete;

    // Setters for components of the camera state.
    void SetPosition(glm::vec3 position);
    void SetTargetPosition(glm::vec3 targetPosition);
    void SetUp(glm::vec3 up);
    void SetUpFallback(glm::vec3 upFallback);
    // This information is used for view and projection matrices.
    // It should always be based on swapchain's extent because that is the thing that matters for rendering.
    // GLFW framebuffer size should be the same as extent, but there might be exceptions.
    // GLFW window size does not have to equal framebuffer size. It is relevant for cursor position for example.
    void SetViewportDimensions(uint32_t width, uint32_t height);

    // Returns position.
    glm::vec3 GetPosition() const;
    // Returns target position.
    glm::vec3 GetTargetPosition() const;
    // Returns target position.
    glm::vec3 GetUp() const;
    // Returns target position.
    glm::vec3 GetUpFallback() const;
    // Returns view matrix.
    glm::mat4 GetViewMatrix() const;
    // Getters for individual components of the projection matrix.
    float GetFieldOfView() const;
    float GetAspectRatio() const;
    float GetNearPlane() const;
    float GetFarPlane() const;
    // Returns perspective projection matrix.
    glm::mat4 GetPerspectiveProjectionMatrix() const;
    // Returns orthographic projection matrix.
    glm::mat4 GetOrthographicProjectionMatrix() const;

    // Updates camera position and target position.
    // Simulates a free camera movement.
    // Expects the mouse to be in a capture mode.
    // WSAD - forward/backwards, strafe left/right
    // SPACE - up
    // LEFT SHIFT - 2x speed
    // Mouse movement - look around
    void InputBasedFreeMovementUpdate(VulkanInputHandler& inputHandler, float deltaTime, float cameraSpeed, float cameraSensitivity);

private:
};
