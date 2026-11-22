
#include "VulkanCamera.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VulkanConfig.h"
#include "VulkanSwapchain.h"
#include "VulkanInputHandler.h"
#include "VulkanHelperFunctions.h"


VulkanCamera::VulkanCamera(const VulkanSwapchain& swapchain, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    position_(visualizerConfig.cameraConfig_.position_),
    targetPosition_(visualizerConfig.cameraConfig_.targetPosition_),
    up_(visualizerConfig.cameraConfig_.up_),
    upFallback_(visualizerConfig.cameraConfig_.upFallback_),
    fieldOfView_(visualizerConfig.cameraConfig_.fieldOfView_),
    aspectRatio_(swapchain.GetAspectRatio()),
    nearPlane_(visualizerConfig.cameraConfig_.nearPlane_),
    farPlane_(visualizerConfig.cameraConfig_.farPlane_),
    viewportWidth_(swapchain.GetExtent().width),
    viewportHeight_(swapchain.GetExtent().height)
{
    LOG_DBG(this->logger_) << "Creating camera. " 
        << "position <" << GetPrintableValue(this->position_) << ">, "
        << "targetPosition <" << GetPrintableValue(this->targetPosition_) << ">, "
        << "up <" << GetPrintableValue(this->up_) << ">, "
        << "upFallback <" << GetPrintableValue(this->upFallback_) << ">, "
        << "fieldOfView <" << this->fieldOfView_ << ">, "
        << "aspectRatio <" << this->aspectRatio_ << ">, "
        << "nearPlane <" << this->nearPlane_ << ">, "
        << "farPlane <" << this->farPlane_ << ">,"
        << "viewportWidth <" << this->viewportWidth_ << ">,"
        << "viewportHeight <" << this->viewportHeight_ << ">" << std::endl;

    LOG_DBG(this->logger_) << "Camera created." << std::endl;
}

VulkanCamera::~VulkanCamera()
{
    LOG_DBG(this->logger_) << "Camera destroyed. " << std::endl;
}

void VulkanCamera::SetPosition(glm::vec3 position)
{
    this->position_ = position;
}

void VulkanCamera::SetTargetPosition(glm::vec3 targetPosition)
{
    this->targetPosition_ = targetPosition;
}

void VulkanCamera::SetUp(glm::vec3 up)
{
    this->up_ = up;
}

void VulkanCamera::SetUpFallback(glm::vec3 upFallback)
{
    this->upFallback_ = upFallback;
}

void VulkanCamera::SetViewportDimensions(uint32_t width, uint32_t height)
{
    this->viewportWidth_ = width;
    this->viewportHeight_ = height;

    this->aspectRatio_ = static_cast<float>(width)/static_cast<float>(height);
}

glm::vec3 VulkanCamera::GetPosition() const
{
    return this->position_;
}

glm::vec3 VulkanCamera::GetTargetPosition() const
{
    return this->targetPosition_;
}

glm::vec3 VulkanCamera::GetUp() const
{
    return this->up_;
}

glm::vec3 VulkanCamera::GetUpFallback() const
{
    return this->upFallback_;
}

glm::mat4 VulkanCamera::GetViewMatrix() const
{
    const glm::vec3 normalizedCameraForwardVector = glm::normalize(this->targetPosition_ - this->position_);

    return glm::lookAt(this->position_, this->targetPosition_, glm::abs(glm::dot(normalizedCameraForwardVector, this->up_)) > 0.99999f ? this->upFallback_ : this->up_);
}

float VulkanCamera::GetFieldOfView() const
{
    return this->fieldOfView_;
}

float VulkanCamera::GetAspectRatio() const
{
    return this->aspectRatio_;
}

float VulkanCamera::GetNearPlane() const
{
    return this->nearPlane_;
}

float VulkanCamera::GetFarPlane() const
{
    return this->farPlane_;
}

glm::mat4 VulkanCamera::GetPerspectiveProjectionMatrix() const
{
    glm::mat4 perspectiveMatrix = glm::perspective(this->fieldOfView_, this->aspectRatio_, this->nearPlane_, this->farPlane_);

    // Flip because of Vulkan - OpenGL difference in convention.
    perspectiveMatrix[1][1] *= -1.0f;

    return perspectiveMatrix;
}

glm::mat4 VulkanCamera::GetOrthographicProjectionMatrix() const
{
    const float halfWidth = static_cast<float>(this->viewportWidth_)*0.5f;
    const float halfHeight = static_cast<float>(this->viewportHeight_)*0.5f;

    glm::mat4 orthographicMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, this->nearPlane_, this->farPlane_);

    orthographicMatrix[1][1] *= -1.0f;

    return orthographicMatrix;
}

void VulkanCamera::InputBasedFreeMovementUpdate(VulkanInputHandler& inputHandler, float deltaTime, float cameraSpeed, float cameraSensitivity)
{
    glm::vec3 currentNormalizedTargetVector = glm::normalize(this->targetPosition_ - this->position_);


    // Pitch and yaw are extracted from the camera position and camera target position.
    // (Pitch and yaw could be permanently stored, but I want this to be a self contained thing for this renderer.)
    // -pi/2 to pi/2 -> just arcsin of the height.
    float pitch = std::asinf(currentNormalizedTargetVector.z);
    // -pi to pi -> x and y are known, but not hypothenuse -> direct arctan.
    // arctan2 extends arctan functionality. By giving it both coordinates instead of a ratio, the quadrant can be determined.
    float yaw = std::atan2f(currentNormalizedTargetVector.x, currentNormalizedTargetVector.y);


    // Pitch and yaw is adjusted based on mouse input.
    double cursorDeltaX = 0.0f;
    double cursorDeltaY = 0.0f;
    inputHandler.GetCursorDelta(cursorDeltaX, cursorDeltaY);

    yaw += static_cast<float>(cursorDeltaX)*cameraSensitivity;

    // Subtraction here, because mouse down is positive delta, but mouse down should correspond to look down.
    pitch -= static_cast<float>(cursorDeltaY)*cameraSensitivity;
    pitch = glm::clamp(pitch, -glm::radians(89.0f), glm::radians(89.0f));

    // Current normalized target vector is updated before camera movement is applied.
    // Height is only dependent on the pitch angle.
    // x and y are dependent on both the pitch and the yaw.
    // std::sinf(yaw) and std::cosf(yaw) specify the lengths of a unit vector lying in a x-y plane,
    // but because our vector must be normalized and has a z component, both of these values
    // must be multiplied by the horizontal distance from the origin, which is given by the std::cosf(pitch).
    // Output vector given by the calculation is normalized.
    currentNormalizedTargetVector.x = std::sinf(yaw)*std::cosf(pitch);
    currentNormalizedTargetVector.y = std::cosf(yaw)*std::cosf(pitch);
    currentNormalizedTargetVector.z = std::sinf(pitch);


    // Camera movement can now be handled.

    glm::vec3 normalizedStrafeLeftVector = glm::normalize(glm::cross(this->up_, currentNormalizedTargetVector));
    glm::vec3 normalizedStrafeRightVector = -1.0f*normalizedStrafeLeftVector;

    bool cameraMoved = false;
    glm::vec3 newPositionVector = {0.0f, 0.0f, 0.0f};

    if (inputHandler.IsKeyDown(GLFW_KEY_W) == true)
    {
        newPositionVector += currentNormalizedTargetVector;
        cameraMoved = true;
    }
    else if (inputHandler.IsKeyDown(GLFW_KEY_S) == true)
    {
        newPositionVector -= currentNormalizedTargetVector;
        cameraMoved = true;
    }

    if (inputHandler.IsKeyDown(GLFW_KEY_A) == true)
    {
        newPositionVector += normalizedStrafeLeftVector;
        cameraMoved = true;
    }
    else if (inputHandler.IsKeyDown(GLFW_KEY_D) == true)
    {
        newPositionVector += normalizedStrafeRightVector;
        cameraMoved = true;
    }

    if (inputHandler.IsKeyDown(GLFW_KEY_SPACE) == true)
    {
        newPositionVector += glm::normalize(this->up_);
        cameraMoved = true;
    }

    newPositionVector = (cameraMoved == true ? glm::normalize(newPositionVector) : newPositionVector)*(inputHandler.IsKeyDown(GLFW_KEY_LEFT_SHIFT) == true ? 2.0f*cameraSpeed : cameraSpeed)*deltaTime;

    this->position_ += newPositionVector;
    this->targetPosition_ = this->position_ + currentNormalizedTargetVector;
}
