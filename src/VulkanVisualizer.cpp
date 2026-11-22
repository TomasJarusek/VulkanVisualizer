
#include "VulkanVisualizer.h"

#include "VulkanHelperEnums.h"
#include "VulkanHelperStructs.h"
#include "VulkanHelperFunctions.h"


// Instantiation of the global dispatcher object, which will hold Vulkan function pointers.
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

VulkanVisualizer::VulkanVisualizer()
{
    // Logger.
    this->visualizerConfig_.SetShowMetaInformation_Logger(false);

    // General.
    // this->visualizerConfig_.SetInitialRendererName("VulkanRenderer");
    this->visualizerConfig_.SetMinimumVulkanVersion(vk::makeVersion(1, 3, 0));

    // Window.
    // this->visualizerConfig_.SetWindowProperties_Window(VulkanWindowMode::Window, 1280, 720, -1, -1);

    // Inputs.
    this->visualizerConfig_.SetCursorIsCaptured_InputHandler(false);

    // Instance.
    #ifdef NDEBUG
        this->visualizerConfig_.SetEnableValidationLayers_Instance(false);
    #else
        this->visualizerConfig_.SetEnableValidationLayers_Instance(true);
        this->visualizerConfig_.SetUseLayerSettings_Instance(true);
        this->visualizerConfig_.SetShowMessengerInfoMessages_Instance(false);
        this->visualizerConfig_.SetShowMessengerVerboseMessages_Instance(false);
    #endif
    this->visualizerConfig_.SetShowAvailableLayersAndExtensions_Instance(false);

    // Device.
    this->visualizerConfig_.SetShowAvailablePhysicalDevices_Device(true);
    
    // Shader module.
    #ifdef NDEBUG
        this->visualizerConfig_.SetCompileShaderWithDebugFlags_ShaderModule(false);
    #else
        this->visualizerConfig_.SetCompileShaderWithDebugFlags_ShaderModule(true);
    #endif
    this->visualizerConfig_.SetCompileShaderAtRuntime_ShaderModule(true);

    // Buffers.
    // 100MB.
    // this->visualizerConfig_.SetBufferSize_Buffer(VulkanBufferUseCase::Vertex, 100000000);
    // this->visualizerConfig_.SetBufferSize_Buffer(VulkanBufferUseCase::Index, 100000000);
    // this->visualizerConfig_.SetBufferSize_Buffer(VulkanBufferUseCase::Staging, 100000000);
    this->visualizerConfig_.SetMaxVertexCount(1000000);

    // Camera.
    this->visualizerConfig_.SetViewMatrixComponents_Camera({0.0f, -5.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
    this->visualizerConfig_.SetPerspectiveMatrixComponents_Camera(glm::radians(60.0f), 0.1f, 200.0f);

    // Light.
    this->visualizerConfig_.SetLightSourceComponents({20.0f, 0.0f, 20.0f}, {1.0f, 1.0f, 1.0f});
    this->visualizerConfig_.SetPhongParameters(true, 0.15f, 64.0f);

    // General drawing.
    this->visualizerConfig_.SetFramesInFlightCount(2);

    this->visualizerConfig_.UseIndexedDraw(false);

    this->visualizerConfig_.UseMSAA(true, vk::SampleCountFlagBits::e8);


    // Logger's settings should be kept internal, so it can be constructed here before Init().
    this->logger_ = std::make_unique<Logger>(this->visualizerConfig_.loggerConfig_.showMetaInformation_);

    LOG_DBG((*this->logger_)) << "Renderer created." << std::endl;
}

VulkanVisualizer::~VulkanVisualizer()
{
    // Explicit wait for the device before renderer destruction (if exists).
    if (this->device_ != nullptr)
    {
        this->device_->WaitIdle();
    }

    LOG_DBG((*this->logger_)) << "Renderer destroyed." << std::endl;
}

void VulkanVisualizer::SetInitialVisualizerMode(bool is2D)
{
    this->visualizerConfig_.SetVisualizerMode(is2D);
}

void VulkanVisualizer::SetInitialRendererName(std::string name)
{
    this->visualizerConfig_.SetRendererName(name);
}

void VulkanVisualizer::SetInitialWindowMode(VulkanWindowMode windowMode, int windowWidth, int windowHeight, int windowX, int windowY)
{
    this->visualizerConfig_.SetWindowProperties_Window(windowMode, windowWidth, windowHeight, windowX, windowY);
}

void VulkanVisualizer::SetInitialMaxVertexCount(size_t maxVertexCount)
{
    this->visualizerConfig_.SetMaxVertexCount(maxVertexCount);
}

void VulkanVisualizer::SetInitialUseMSAA(bool useMSAA, size_t msaaSampleCount)
{
    vk::SampleCountFlagBits sampleCountFlagBits = vk::SampleCountFlagBits::e8;

    if (msaaSampleCount == 2)
    {
        sampleCountFlagBits = vk::SampleCountFlagBits::e2;
    }
    else if (msaaSampleCount == 4)
    {
        sampleCountFlagBits = vk::SampleCountFlagBits::e4;
    }
    else if (msaaSampleCount == 8)
    {
        sampleCountFlagBits = vk::SampleCountFlagBits::e8;
    }

    this->visualizerConfig_.UseMSAA(useMSAA, sampleCountFlagBits);
}

void VulkanVisualizer::SetInitialShowLiveTimingInfo(bool showLiveTimingInfo)
{
    this->visualizerConfig_.SetShowLiveTimingInfo(showLiveTimingInfo);
}

void VulkanVisualizer::SetInitialShowLiveFrameInfo(bool showLiveFrameInfo)
{
    this->visualizerConfig_.SetShowLiveFrameInfo(showLiveFrameInfo);
}

void VulkanVisualizer::SetInitialShowPostCloseTimingInfo(bool showPostCloseTimingInfo)
{
    this->visualizerConfig_.SetShowPostCloseTimingInfo(showPostCloseTimingInfo);
}

void VulkanVisualizer::Init()
{
    // Initialization of independent function pointers of Vulkan functions.
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    // Renderer's internal config is initialized.
    // Any external update to the values will change this->internalConfig_ itself.
    this->internalConfig_ = this->visualizerConfig_.internalConfig_;

    this->window_ = std::make_unique<VulkanWindow>(this->visualizerConfig_);

    this->inputHandler_ = std::make_unique<VulkanInputHandler>(*this->window_, this->visualizerConfig_);
    
    this->callbackDispatcher_ = std::make_unique<VulkanCallbackDispatcher>(*this->window_, *this->inputHandler_);

    this->instance_ = std::make_unique<VulkanInstance>(this->visualizerConfig_);

    this->surface_ = std::make_unique<VulkanSurface>(*this->window_, *this->instance_, this->visualizerConfig_);

    this->device_ = std::make_unique<VulkanDevice>(*this->instance_, *this->surface_, this->visualizerConfig_);
    this->ChooseDepthFormat();

    this->vma_ = std::make_unique<VulkanVMA>(*this->instance_, *this->device_, this->visualizerConfig_);

    this->swapchain_ = std::make_unique<VulkanSwapchain>(*this->window_, *this->surface_, *this->device_, this->visualizerConfig_);

    this->descriptorPool_ = std::make_unique<VulkanDescriptorPool>(*this->device_, this->visualizerConfig_);

    this->vertexShaderModule_ = std::make_unique<VulkanShaderModule>("../src/shaders/shader.vert", *this->device_, this->visualizerConfig_);
    this->fragmentShaderModule_ = std::make_unique<VulkanShaderModule>("../src/shaders/shader.frag", *this->device_, this->visualizerConfig_);

    this->globalDataDescriptorSetLayout_ = std::make_unique<VulkanDescriptorSetLayout>(*this->device_, this->PrepareGlobalDataDescriptorSetLayoutBindings(), this->visualizerConfig_);
    this->descriptorSetLayouts_.push_back(*this->globalDataDescriptorSetLayout_);

    this->pipelineLayout_ = std::make_unique<VulkanPipelineLayout>(*this->device_, this->descriptorSetLayouts_, this->visualizerConfig_);

    this->pipelineBuilder_ = std::make_unique<VulkanPipelineBuilder>(*this->device_, this->visualizerConfig_);
    this->opaquePipeline_ = this->BuildOpaquePipeline();
    this->transparentPipeline_ = this->BuildTransparentPipeline();

    this->frameContexts_.reserve(this->internalConfig_.framesInFlightCount_);
    for (size_t i = 0; i < this->internalConfig_.framesInFlightCount_; ++i)
    {
        this->frameContexts_.emplace_back(std::make_unique<VulkanFrameContext>(*this->device_, *this->swapchain_, *this->vma_, *this->descriptorPool_, this->descriptorSetLayouts_, this->internalConfig_, this->visualizerConfig_));
    }

    this->camera_ = std::make_unique<VulkanCamera>(*this->swapchain_, this->visualizerConfig_);

    LOG_DBG((*this->logger_)) << "Renderer initialized." << std::endl;
}

void VulkanVisualizer::SetWindowMode(VulkanWindowMode windowMode, int windowWidth, int windowHeight, int windowX, int windowY)
{
    this->window_->SetWindowMode(windowMode, windowWidth, windowHeight, windowX, windowY);
}

void VulkanVisualizer::SetBackgroundColor(float colorR, float colorG, float colorB)
{
    this->internalConfig_.backgroundColor_ = {colorR, colorG, colorB, 1.0f};
}

void VulkanVisualizer::SetCameraPosition(float positionX, float positionY, float positionZ)
{
    this->camera_->SetPosition({positionX, positionY, positionZ});
}

void VulkanVisualizer::SetCameraTargetPosition(float targetPositionX, float targetPositionY, float targetPositionZ)
{
    this->camera_->SetTargetPosition({targetPositionX, targetPositionY, targetPositionZ});
}

void VulkanVisualizer::SetCameraUp(float upX, float upY, float upZ)
{
    this->camera_->SetUp({upX, upY, upZ});
}

void VulkanVisualizer::SetCameraUpFallback(float upFallbackX, float upFallbackY, float upFallbackZ)
{
    this->camera_->SetUpFallback({upFallbackX, upFallbackY, upFallbackZ});
}

void VulkanVisualizer::GetCameraPosition(float& positionX, float& positionY, float& positionZ) const
{
    glm::vec3 cameraPosition = this->camera_->GetPosition();

    positionX = cameraPosition.x;
    positionY = cameraPosition.y;
    positionZ = cameraPosition.z;
}

void VulkanVisualizer::GetCameraTargetPosition(float& targetPositionX, float& targetPositionY, float& targetPositionZ) const
{
    glm::vec3 cameraTargetPosition = this->camera_->GetTargetPosition();

    targetPositionX = cameraTargetPosition.x;
    targetPositionY = cameraTargetPosition.y;
    targetPositionZ = cameraTargetPosition.z;
}

void VulkanVisualizer::GetCameraUp(float& upX, float& upY, float& upZ) const
{
    glm::vec3 cameraUp = this->camera_->GetUp();

    upX = cameraUp.x;
    upY = cameraUp.y;
    upZ = cameraUp.z;
}

void VulkanVisualizer::GetCameraUpFallback(float& upFallbackX, float& upFallbackY, float& upFallbackZ) const
{
    glm::vec3 cameraUpFallback = this->camera_->GetUpFallback();

    upFallbackX = cameraUpFallback.x;
    upFallbackY = cameraUpFallback.y;
    upFallbackZ = cameraUpFallback.z;
}

void VulkanVisualizer::GetCameraPerspectiveMatrixComponents(float& fieldOfView, float& aspectRatio, float& nearPlane, float& farPlane) const
{
    fieldOfView = this->camera_->GetFieldOfView();
    aspectRatio = this->camera_->GetAspectRatio();
    nearPlane = this->camera_->GetNearPlane();
    farPlane = this->camera_->GetFarPlane();
}

void VulkanVisualizer::SetLightSourcePosition(float positionX, float positionY, float positionZ)
{
    this->internalConfig_.lightSourcePosition_ = {positionX, positionY, positionZ, 0.0f};
}

void VulkanVisualizer::SetLightSourceColor(float colorR, float colorG, float colorB)
{
    this->internalConfig_.lightSourceColor_ = {colorR, colorG, colorB, 0.0f};
}

void VulkanVisualizer::SetUsePhongReflectionModel(bool usePhongReflectionModel)
{
    this->internalConfig_.phongParameters_.x = static_cast<float>(usePhongReflectionModel);
}

void VulkanVisualizer::SetPhongAmbient(float phongAmbient)
{
    this->internalConfig_.phongParameters_.y = phongAmbient;
}

void VulkanVisualizer::SetPhongShininess(float phongShininess)
{
    this->internalConfig_.phongParameters_.z = phongShininess;
}

void VulkanVisualizer::SetCursorIsCaptured(bool cursorIsCaptured)
{
    this->inputHandler_->SetCursorIsCaptured(cursorIsCaptured);
}

bool VulkanVisualizer::IsKeyDown(int glfwKey) const
{
    return this->inputHandler_->IsKeyDown(glfwKey);
}

bool VulkanVisualizer::IsKeyJustPressed(int glfwKey) const
{
    return this->inputHandler_->IsKeyJustPressed(glfwKey);
}

bool VulkanVisualizer::IsKeyJustReleased(int glfwKey) const
{
    return this->inputHandler_->IsKeyJustReleased(glfwKey);
}

bool VulkanVisualizer::IsButtonDown(int glfwButton) const
{
    return this->inputHandler_->IsButtonDown(glfwButton);
}

bool VulkanVisualizer::IsButtonJustPressed(int glfwButton) const
{
    return this->inputHandler_->IsButtonJustPressed(glfwButton);
}

bool VulkanVisualizer::IsButtonJustReleased(int glfwButton) const
{
    return this->inputHandler_->IsButtonJustReleased(glfwButton);
}

void VulkanVisualizer::GetCursorPos(double& cursorX, double& cursorY) const
{
    this->inputHandler_->GetCursorPos(cursorX, cursorY);
}

void VulkanVisualizer::GetCursorDelta(double& cursorDeltaX, double& cursorDeltaY) const
{
    this->inputHandler_->GetCursorDelta(cursorDeltaX, cursorDeltaY);
}

void VulkanVisualizer::GetScrollDelta(double& scrollDeltaX, double& scrollDeltaY) const
{
    this->inputHandler_->GetScrollDelta(scrollDeltaX, scrollDeltaY);
}

void VulkanVisualizer::InputBasedFreeMovementCameraUpdate(float cameraSpeed, float cameraSensitivity)
{
    this->camera_->InputBasedFreeMovementUpdate(*this->inputHandler_, this->GetDeltaTime<float>(), cameraSpeed, cameraSensitivity);
}

std::pair<int, int> VulkanVisualizer::GetWindowSize() const
{
    return this->window_.get()->GetWindowSize();
}

std::pair<int, int> VulkanVisualizer::GetFramebufferSize() const
{
    return this->window_.get()->GetFramebufferSize();
}

void VulkanVisualizer::Draw3DTriangle(float firstVertexX, float firstVertexY, float firstVertexZ, float secondVertexX, float secondVertexY, float secondVertexZ, float thirdVertexX, float thirdVertexY, float thirdVertexZ, float colorR, float colorG, float colorB, float colorA)
{
    this->DrawTriangle({firstVertexX, firstVertexY, firstVertexZ}, {secondVertexX, secondVertexY, secondVertexZ}, {thirdVertexX, thirdVertexY, thirdVertexZ}, {colorR, colorG, colorB, colorA});
}

void VulkanVisualizer::Draw3DIcosahedron(float centerX, float centerY, float centerZ, float colorR, float colorG, float colorB, float colorA, float diameter)
{
    static constexpr float phi = std::numbers::phi_v<float>;
    static constexpr float icosahedronCircumradius = ConstexprSqrt<float>(1.0f + phi*phi);

    // Icosahedron vertices.
    // (0, +-1, +-phi)
    // (+-1, +-phi, 0)
    // (+-phi, 0, +-1)
    static constexpr std::array<glm::vec3, 12> baseVertices =
    {
        glm::vec3{-1.0f, phi, 0.0f},
        glm::vec3{1.0f, phi, 0.0f},
        glm::vec3{-1.0f, -phi, 0.0f},
        glm::vec3{1.0f, -phi, 0.0f},
        glm::vec3{0.0f, -1.0f, phi},
        glm::vec3{0.0f, 1.0f, phi},
        glm::vec3{0.0f, -1.0f, -phi},
        glm::vec3{0.0f, 1.0f, -phi},
        glm::vec3{phi, 0.0f, -1.0f},
        glm::vec3{phi, 0.0f, 1.0f},
        glm::vec3{-phi, 0.0f, -1.0f},
        glm::vec3{-phi, 0.0f, 1.0f}
    };

    // Faces constructed from indexes of vertices.
    static constexpr std::array<std::array<std::size_t, 3>, 20> triangleIndexes =
    {{
        {0, 11, 5},
        {0, 5, 1},
        {0, 1, 7},
        {0, 7, 10},
        {0, 10, 11},
        {1, 5, 9},
        {5, 11, 4},
        {11, 10, 2},
        {10, 7, 6},
        {7, 1, 8},
        {3, 9, 4},
        {3, 4, 2},
        {3, 2, 6},
        {3, 6, 8},
        {3, 8, 9},
        {4, 9, 5},
        {2, 4, 11},
        {6, 2, 10},
        {8, 6, 7},
        {9, 8, 1}
    }};

    // One normal per triangle.
    static constexpr std::array<glm::vec3, triangleIndexes.size()> baseNormals = []
    {
        std::array<glm::vec3, triangleIndexes.size()> normals{};
    
        for (std::size_t i = 0; i < triangleIndexes.size(); ++i)
        {
            const std::array<std::size_t, 3>& currentTriangleIndexes = triangleIndexes[i];
    
            const std::array<float, 3> firstVertex{baseVertices[currentTriangleIndexes[0]].x, baseVertices[currentTriangleIndexes[0]].y, baseVertices[currentTriangleIndexes[0]].z};
            const std::array<float, 3> secondVertex{baseVertices[currentTriangleIndexes[1]].x, baseVertices[currentTriangleIndexes[1]].y, baseVertices[currentTriangleIndexes[1]].z};
            const std::array<float, 3> thirdVertex{baseVertices[currentTriangleIndexes[2]].x, baseVertices[currentTriangleIndexes[2]].y, baseVertices[currentTriangleIndexes[2]].z};
    
            normals[i] = Convert<float, glm::vec3, 3>(Normalize(Cross(Subtract(secondVertex, firstVertex), Subtract(thirdVertex, firstVertex))));
        }
    
        return normals;
    }();

    const glm::vec3 center{centerX, centerY, centerZ};
    const glm::vec4 color{colorR, colorG, colorB, colorA};

    const float radius = diameter/2.0f;
    const float scale = radius/icosahedronCircumradius;

    // Base vertices are scale and translated.
    std::array<glm::vec3, baseVertices.size()> vertices{};

    for (std::size_t i = 0; i < baseVertices.size(); ++i)
    {
        vertices[i] = center + baseVertices[i]*scale;
    }

    // Construct each triangle using the indexes
    for (std::size_t i = 0; i < triangleIndexes.size(); ++i)
    {
        const std::array<std::size_t, 3>& currentTriangleIndexes = triangleIndexes[i];

        this->DrawTriangle(vertices[currentTriangleIndexes[0]], vertices[currentTriangleIndexes[1]], vertices[currentTriangleIndexes[2]], color, baseNormals[i]);
    }
}

void VulkanVisualizer::Draw3DCube(float centerX, float centerY, float centerZ, float colorR, float colorG, float colorB, float colorA, float sideLength)
{
    static constexpr float cubeSideLength = 2.0f;

    // Vertices.
    static constexpr std::array<glm::vec3, 8> baseVertices =
    {
        glm::vec3{-1.0f, -1.0f, -1.0f}, 
        glm::vec3{1.0f, -1.0f, -1.0f},
        glm::vec3{1.0f, 1.0f, -1.0f},
        glm::vec3{-1.0f, 1.0f, -1.0f},
        glm::vec3{-1.0f, -1.0f, 1.0f},
        glm::vec3{1.0f, -1.0f, 1.0f},
        glm::vec3{1.0f, 1.0f, 1.0f},
        glm::vec3{-1.0f, 1.0f, 1.0f}
    };

    // Faces constructed from indexes of vertices.
    static constexpr std::array<std::array<std::size_t, 3>, 12> triangleIndexes =
    {{
        {7, 6, 2},
        {2, 3, 7},
        {0, 1, 5},
        {5, 4, 0},
        {5, 1, 2},
        {2, 6, 5},
        {3, 0, 4},
        {4, 7, 3},
        {4, 5, 6},
        {6, 7, 4},
        {0, 2, 1},
        {0, 3, 2}
    }};

    // One normal per triangle.
    static constexpr std::array<glm::vec3, triangleIndexes.size()> baseNormals = []
    {
        std::array<glm::vec3, triangleIndexes.size()> normals{};
    
        for (std::size_t i = 0; i < triangleIndexes.size(); ++i)
        {
            const std::array<std::size_t, 3>& currentTriangleIndexes = triangleIndexes[i];
    
            const std::array<float, 3> firstVertex{baseVertices[currentTriangleIndexes[0]].x, baseVertices[currentTriangleIndexes[0]].y, baseVertices[currentTriangleIndexes[0]].z};
            const std::array<float, 3> secondVertex{baseVertices[currentTriangleIndexes[1]].x, baseVertices[currentTriangleIndexes[1]].y, baseVertices[currentTriangleIndexes[1]].z};
            const std::array<float, 3> thirdVertex{baseVertices[currentTriangleIndexes[2]].x, baseVertices[currentTriangleIndexes[2]].y, baseVertices[currentTriangleIndexes[2]].z};
    
            normals[i] = Convert<float, glm::vec3, 3>(Normalize(Cross(Subtract(secondVertex, firstVertex), Subtract(thirdVertex, firstVertex))));
        }
    
        return normals;
    }();

    const glm::vec3 center{centerX, centerY, centerZ};
    const glm::vec4 color{colorR, colorG, colorB, colorA};

    const float scale = sideLength/cubeSideLength;

    // Base vertices are scale and translated.
    std::array<glm::vec3, baseVertices.size()> vertices{};

    for (std::size_t i = 0; i < baseVertices.size(); ++i)
    {
        vertices[i] = center + baseVertices[i]*scale;
    }

    // Construct each triangle using the idexes
    for (std::size_t i = 0; i < triangleIndexes.size(); ++i)
    {
        const std::array<std::size_t, 3>& currentTriangleIndexes = triangleIndexes[i];

        this->DrawTriangle(vertices[currentTriangleIndexes[0]], vertices[currentTriangleIndexes[1]], vertices[currentTriangleIndexes[2]], color, baseNormals[i]);
    }
}

void VulkanVisualizer::Draw3DBox(float originX, float originY, float originZ, float rightX, float rightY, float rightZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ, float colorR, float colorG, float colorB, float colorA)
{
    const glm::vec3 originPoint = {originX, originY, originZ};
    const glm::vec3 rightPoint = {rightX, rightY, rightZ};
    const glm::vec3 forwardPoint = {forwardX, forwardY, forwardZ};
    const glm::vec3 upPoint = {upX, upY, upZ};
    const glm::vec4 color = {colorR, colorG, colorB, colorA};

    const glm::vec3 rightForwardPoint = rightPoint + forwardPoint - originPoint;
    const glm::vec3 rightUpPoint = rightPoint + upPoint - originPoint;
    const glm::vec3 forwardUpPoint = forwardPoint + upPoint - originPoint;
    const glm::vec3 oppositeCornerPoint = rightPoint + forwardPoint + upPoint - 2.0f*originPoint;

    const glm::vec3 rightVector = rightPoint - originPoint;
    const glm::vec3 forwardVector = forwardPoint - originPoint;
    const glm::vec3 upVector = upPoint - originPoint;

    const glm::vec3 centerPoint = originPoint + (rightVector + forwardVector + upVector)*0.5f;

    // Degenerate cuboids are rejected.
    const float volume = glm::dot(rightVector, glm::cross(forwardVector, upVector));

    if (std::abs(volume) <= 1e-6f)
    {
        return;
    }

    // Because this function does not transform an existing object, but allows any points to be passed in,
    // the winding of the triangles must be checked to ensure their normals point out of the cuboid.
    const auto drawTriangleCCW = [this, &color](const glm::vec3& firstVertex, const glm::vec3& secondVertex, const glm::vec3& thirdVertex, const glm::vec3& outwardVector)
    {
        const glm::vec3 normalVector = glm::cross(secondVertex - firstVertex, thirdVertex - firstVertex);

        if (glm::dot(normalVector, outwardVector) < 0.0f)
        {
            this->DrawTriangle(firstVertex, thirdVertex, secondVertex, color, -normalVector);
        }
        else
        {
            this->DrawTriangle(firstVertex, secondVertex, thirdVertex, color, normalVector);
        }
    };

    // Outward facing vector don't need to be normals, they just have to point toward the right half of 3D space.
    // centerPoint -> originPoint vector and centerPoint -> oppositeCornerPoint will be used for their three adjacent faces respectively.

    drawTriangleCCW(originPoint, forwardPoint, rightForwardPoint, originPoint - centerPoint);
    drawTriangleCCW(originPoint, rightForwardPoint, rightPoint, originPoint - centerPoint);

    drawTriangleCCW(upPoint, rightUpPoint, oppositeCornerPoint, oppositeCornerPoint - centerPoint);
    drawTriangleCCW(upPoint, oppositeCornerPoint, forwardUpPoint, oppositeCornerPoint - centerPoint);

    drawTriangleCCW(originPoint, rightPoint, rightUpPoint, originPoint - centerPoint);
    drawTriangleCCW(originPoint, rightUpPoint, upPoint, originPoint - centerPoint);

    drawTriangleCCW(forwardPoint, rightForwardPoint, oppositeCornerPoint, oppositeCornerPoint - centerPoint);
    drawTriangleCCW(forwardPoint, oppositeCornerPoint, forwardUpPoint, oppositeCornerPoint - centerPoint);

    drawTriangleCCW(originPoint, forwardPoint, forwardUpPoint, originPoint - centerPoint);
    drawTriangleCCW(originPoint, forwardUpPoint, upPoint, originPoint - centerPoint);

    drawTriangleCCW(rightPoint, rightUpPoint, oppositeCornerPoint, oppositeCornerPoint - centerPoint);
    drawTriangleCCW(rightPoint, oppositeCornerPoint, rightForwardPoint, oppositeCornerPoint - centerPoint);
}

void VulkanVisualizer::Draw3DRectangularCuboid(float originX, float originY, float originZ, float oppositeX, float oppositeY, float oppositeZ, float colorR, float colorG, float colorB, float colorA)
{
    this->Draw3DBox(originX, originY, originZ, oppositeX, originY, originZ, originX, oppositeY, originZ, originX, originY, oppositeZ, colorR, colorG, colorB, colorA);
}

void VulkanVisualizer::Draw2DTriangle(float firstVertexX, float firstVertexY, float secondVertexX, float secondVertexY, float thirdVertexX, float thirdVertexY, float Z, float colorR, float colorG, float colorB, float colorA)
{
    const glm::vec3 firstVertex = {firstVertexX, firstVertexY, Z};
    const glm::vec3 secondVertex = {secondVertexX, secondVertexY, Z};
    const glm::vec3 thirdVertex = {thirdVertexX, thirdVertexY, Z};

    const glm::vec4 color = {colorR, colorG, colorB, colorA};

    // Vertex order will be adjusted so that any triangle will be visible.
    const glm::vec3 normalVector = glm::cross(secondVertex - firstVertex, thirdVertex - firstVertex);

    if (glm::dot(normalVector, this->internalConfig_.up_) >= 0.0f)
    {
        this->DrawTriangle(firstVertex, secondVertex, thirdVertex, color, normalVector);
    }
    else
    {
        this->DrawTriangle(firstVertex, thirdVertex, secondVertex, color, -normalVector);
    }
}

void VulkanVisualizer::Draw2DLineSegment(float startX, float startY, float endX, float endY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness, float capOffset)
{
    const glm::vec2 startPoint = {startX, startY};
    const glm::vec2 endPoint = {endX, endY};

    const glm::vec2 startEndVector = endPoint - startPoint;
    const float length = glm::length(startEndVector);

    // Only positive length line segments with positive thickness are valid.
    if (length <= 2.0f*capOffset || thickness <= 0.0f)
    {
        return;
    }

    const glm::vec2 normalizedStartEndVector = startEndVector/length;
    const glm::vec2 normalizedRightVector(-normalizedStartEndVector.y, normalizedStartEndVector.x);

    // Both endpoints are offset along the segment direction.
    const glm::vec2 adjustedStartPoint = startPoint + normalizedStartEndVector*capOffset;
    const glm::vec2 adjustedEndPoint = endPoint - normalizedStartEndVector*capOffset;

    // Rectangle's rightPoint vector perpendicular to the segment.
    const glm::vec2 rightVector = normalizedRightVector*(thickness*0.5f);

    const glm::vec2 originPoint = adjustedStartPoint - rightVector;
    const glm::vec2 rightPoint = adjustedEndPoint - rightVector;
    const glm::vec2 forwardPoint = adjustedStartPoint + rightVector;

    this->Draw2DParallelogram(originPoint.x, originPoint.y, rightPoint.x, rightPoint.y, forwardPoint.x, forwardPoint.y, Z, colorR, colorG, colorB, colorA);
}

void VulkanVisualizer::Draw2DPolyline(std::span<const std::array<float, 2>> points, float Z, float colorR, float colorG, float colorB, float colorA, float thickness, bool isLoop)
{
    static constexpr float epsilon = 1e-6f;

    if (points.size() < 2 || thickness <= 0.0f)
    {
        return;
    }

    const size_t pointCount = points.size();
    const size_t segmentCount = isLoop ? pointCount : pointCount - 1;
    const float halfThickness = thickness*0.5f;

    std::vector<glm::vec2> glmPoints = points | std::views::transform([](const std::array<float, 2>& point){return glm::vec2(point[0], point[1]);}) | std::ranges::to<std::vector<glm::vec2>>();

    std::vector<glm::vec2> segmentDirectionVectors(segmentCount);

    // No adjacent points can be identical.
    // This will save us from degeneracy test later.
    for (size_t i = 0; i < segmentCount; ++i)
    {
        const size_t nextIndex = (i + 1) % pointCount;
        const glm::vec2 difference = glmPoints[nextIndex] - glmPoints[i];

        if (glm::dot(difference, difference) <= epsilon*epsilon)
        {
            return;
        }

        // Direction vectors are constructed as well.
        segmentDirectionVectors[i] = glm::normalize(difference);
    }

    // Bisectors construction.
    std::vector<std::pair<glm::vec2, glm::vec2>> bisectors(pointCount);

    for (size_t i = 0; i < pointCount; ++i)
    {
        glm::vec2 bisectorVector;

        if (isLoop == false && i == 0)
        {
            bisectorVector = segmentDirectionVectors[0];
        }
        else if (isLoop == false && i == pointCount - 1)
        {
            bisectorVector = segmentDirectionVectors[segmentCount - 1];
        }
        else
        {
            const size_t previousSegmentIndex = (i + segmentCount - 1) % segmentCount;
            const size_t nextSegmentIndex = i % segmentCount;

            bisectorVector = segmentDirectionVectors[previousSegmentIndex] + segmentDirectionVectors[nextSegmentIndex];

            if (glm::dot(bisectorVector, bisectorVector) <= epsilon*epsilon)
            {
                bisectorVector = segmentDirectionVectors[nextSegmentIndex];
            }
            else
            {
                bisectorVector = glm::normalize(bisectorVector);
            }
        }

        const glm::vec2 bisectorNormalVector(-bisectorVector.y, bisectorVector.x);
        const glm::vec2 offsetVector = bisectorNormalVector*halfThickness;

        bisectors[i].first = glmPoints[i] + offsetVector;
        bisectors[i].second = glmPoints[i] - offsetVector;
    }

    for (size_t i = 0; i < segmentCount; ++i)
    {
        const size_t j = (i + 1) % pointCount;

        this->Draw2DTriangle(bisectors[i].first.x, bisectors[i].first.y, bisectors[j].second.x, bisectors[j].second.y, bisectors[i].second.x, bisectors[i].second.y,Z, colorR, colorG, colorB, colorA);
        this->Draw2DTriangle(bisectors[i].first.x, bisectors[i].first.y, bisectors[j].first.x, bisectors[j].first.y, bisectors[j].second.x, bisectors[j].second.y, Z, colorR, colorG, colorB, colorA);
    }

    if (isLoop == false)
    {
        this->Draw2DTriangle(glmPoints.front().x, glmPoints.front().y, bisectors.front().first.x, bisectors.front().first.y, bisectors.front().second.x, bisectors.front().second.y, Z, colorR, colorG, colorB, colorA);
        this->Draw2DTriangle(glmPoints.back().x, glmPoints.back().y, bisectors.back().second.x, bisectors.back().second.y, bisectors.back().first.x, bisectors.back().first.y, Z, colorR, colorG, colorB, colorA);
    }
}

void VulkanVisualizer::Draw2DParallelogram(float originX, float originY, float rightX, float rightY, float forwardX, float forwardY, float Z, float colorR, float colorG, float colorB, float colorA)
{
    const glm::vec3 originPoint = {originX, originY, Z};
    const glm::vec3 rightPoint = {rightX, rightY, Z};
    const glm::vec3 forwardPoint = {forwardX, forwardY, Z};

    const glm::vec3 oppositeCornerPoint = rightPoint + forwardPoint - originPoint;

    const glm::vec4 color = {colorR, colorG, colorB, colorA};

    // Vertex order will be adjusted so that any triangle will be visible.
    const glm::vec3 normalVector = glm::cross(rightPoint - originPoint, oppositeCornerPoint - originPoint);

    if (glm::dot(normalVector, this->internalConfig_.up_) >= 0.0f)
    {
        this->DrawTriangle(originPoint, rightPoint, oppositeCornerPoint, color, normalVector);
        this->DrawTriangle(originPoint, oppositeCornerPoint, forwardPoint, color, normalVector);
    }
    else
    {
        this->DrawTriangle(originPoint, oppositeCornerPoint, rightPoint, color, -normalVector);
        this->DrawTriangle(originPoint, forwardPoint, oppositeCornerPoint, color, -normalVector);
    }
}

void VulkanVisualizer::Draw2DRectangle(float originX, float originY, float oppositeX, float oppositeY, float Z, float colorR, float colorG, float colorB, float colorA)
{
    this->Draw2DParallelogram(originX, originY, oppositeX, originY, originX, oppositeY, Z, colorR, colorG, colorB, colorA);
}

void VulkanVisualizer::Draw2DArrow(float startX, float startY, float endX, float endY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness)
{
    const glm::vec2 startPoint = {startX, startY};
    const glm::vec2 endPoint = {endX, endY};

    const glm::vec2 startEndVector = endPoint - startPoint;
    const float arrowLength = glm::length(startEndVector);

    if (arrowLength <= 0.0f || thickness <= 0.0f)
    {
        return;
    }

    const glm::vec2 normalizedStartEndVector = startEndVector/arrowLength;
    const glm::vec2 normalizedRightVector(-normalizedStartEndVector.y, normalizedStartEndVector.x);

    const float radius = thickness*0.5f;
    const float tipRadius = radius*2.0f;

    // The length of the tip of the arrow is proportionally bounded by radius and total length.
    // If the arrow is too long, it is bounded by the radius.
    // If the arrow is too short, it is bounded by the arrow length.
    const float tipLength = std::min(radius*8.0f, arrowLength*0.4f);

    const glm::vec2 tipBasePoint = endPoint - normalizedStartEndVector*tipLength;

    // The body is drawn.
    this->Draw2DLineSegment(startPoint.x, startPoint.y, tipBasePoint.x, tipBasePoint.y, Z, colorR, colorG, colorB, colorA, thickness);

    // The arrowhead's two base vertices are calculated.
    const glm::vec2 tipOffsetVector = normalizedRightVector*tipRadius;
    const glm::vec2 tipBaseLeftPoint = tipBasePoint - tipOffsetVector;
    const glm::vec2 tipBaseRightPoint = tipBasePoint + tipOffsetVector;

   // The arrowhead is drawn.
    this->Draw2DTriangle(tipBaseLeftPoint.x, tipBaseLeftPoint.y, tipBaseRightPoint.x, tipBaseRightPoint.y, endPoint.x, endPoint.y, Z, colorR, colorG, colorB, colorA);
}

void VulkanVisualizer::Draw2DAxes(float originX, float originY, float Z, float length, float thickness, float colorA)
{
    const float halfThickness = thickness/2.0f;

    const glm::vec2 rightArrowStartPoint = {originX + halfThickness, originY};
    const glm::vec2 rightArrowEndPoint = {originX + this->internalConfig_.right_.x*length + halfThickness, originY};
    const glm::vec2 forwardArrowStartPoint = {originX, originY + halfThickness};
    const glm::vec2 forwardArrowEndPoint = {originX, originY + this->internalConfig_.forward_.y*length + halfThickness};

    this->Draw2DArrow(rightArrowStartPoint.x, rightArrowStartPoint.y, rightArrowEndPoint.x, rightArrowEndPoint.y , Z, 1.0f, 0.0f, 0.0f, colorA, thickness);
    this->Draw2DTriangle(originX, originY, rightArrowStartPoint.x, rightArrowStartPoint.y + halfThickness, rightArrowStartPoint.x, rightArrowStartPoint.y - halfThickness, Z, 1.0f, 0.0f, 0.0f, colorA);

    this->Draw2DArrow(forwardArrowStartPoint.x, forwardArrowStartPoint.y, forwardArrowEndPoint.x, forwardArrowEndPoint.y, Z, 0.0f, 0.0f, 1.0f, colorA, thickness);
    this->Draw2DTriangle(originX, originY, forwardArrowStartPoint.x + halfThickness, forwardArrowStartPoint.y, forwardArrowStartPoint.x  - halfThickness, forwardArrowStartPoint.y, Z, 0.0f, 0.0f, 1.0f, colorA);
}

bool VulkanVisualizer::WindowShouldClose() const
{
    if (this->window_->WindowShouldClose() == true || this->inputHandler_->IsKeyJustPressed(GLFW_KEY_ESCAPE) == true)
    {
        this->PrintPostCloseTimingInfo();

        return true;
    }
    else
    {
        return false;
    } 
}

void VulkanVisualizer::HandleFrameStart()
{
    this->UpdateTimingVariables();
    this->PrintLiveTimingInfo();

    this->HandleEvents();

    // All vertex data are reset.
    this->vertices_.clear();
    this->transparentTriangleIndexes_.clear();

    if (this->internalConfig_.useIndexedDraw_ == true)
    {
        this->indexes_.clear();
        InitVerticesMap(this->uniqueVertices_);
    }

    this->userWorkloadStart_= std::chrono::high_resolution_clock::now();
}

void VulkanVisualizer::DrawFrame()
{
    this->userWorkloadEnd_ = std::chrono::high_resolution_clock::now();

    // Accumulated transparent triangles are handled at once at the end of the frame.
    this->ProcessTransparentTriangles();

    this->frameBeforeFence_ = std::chrono::high_resolution_clock::now();

    // Resources relevant to the current frame are retrieved.
    VulkanFrameContext& currentFrameContext = *(this->frameContexts_[this->internalConfig_.currentFrameInFlightIndex_]);

    // To start working on this frame, the frame in flight must be ready.
    // We are waiting on a single fence.
    // vk::True indicates that the function should wait on all fences (only one here, so it doesn't matter).
    // uint64_t max value indicates no timeout.
    vk::Result waitForFencesResult = this->device_->Get().waitForFences(currentFrameContext.GetFrameInFlightFence(), vk::True, std::numeric_limits<std::uint64_t>::max());

    this->frameFenceThrough_ = std::chrono::high_resolution_clock::now();

    if (waitForFencesResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("Exception: Wait for a frame in flight fence failed.");
    }

    // Image is acquired from the swapchain.
    auto [acquireNextImageResult, acquiredSwapchainImageIndex] = this->swapchain_->AcquireNextImage(currentFrameContext.GetImageAvailableSemaphore());

    if (acquireNextImageResult == vk::Result::eErrorOutOfDateKHR)
    {
        LOG_DBG((*this->logger_)) << "Swapchain needs to be recreated because of acquireNextImageResult being <" << GetPrintableValue(acquireNextImageResult) << ">." << std::endl;

        this->RecreateSwapchain();

        // Try this frame in flight again.
        return;
    }
    else if (acquireNextImageResult != vk::Result::eSuccess && acquireNextImageResult != vk::Result::eSuboptimalKHR)
    {
        // Anything other than vk::Result::eSuccess and vk::Result::eSuboptimalKHR (image was still retrieved) is an error.

        throw std::runtime_error("Exception: Swapchain image could not be acquired. acquireNextImageResult <" + GetPrintableValue(acquireNextImageResult) + ">");
    }

    // Fence is reset (closed) only once the work is submitted.
    this->device_->Get().resetFences(currentFrameContext.GetFrameInFlightFence());

    if (this->device_->DoTransferAndGraphicsQueuesShareQueueFamily() == true)
    {
        this->HandleTransferAndGraphics(currentFrameContext, acquiredSwapchainImageIndex);
    }
    else
    {
        this->HandleTransfer(currentFrameContext);

        this->HandleGraphics(currentFrameContext, acquiredSwapchainImageIndex);
    }

    // Image is presented.
    vk::Result presentationResult = this->swapchain_->PresentImage(acquiredSwapchainImageIndex);

    if (presentationResult == vk::Result::eSuboptimalKHR || presentationResult == vk::Result::eErrorOutOfDateKHR || this->window_->GetFramebufferResized() == true)
    {
        LOG_DBG((*this->logger_)) << "Swapchain needs to be recreated. presentationResult <" << GetPrintableValue(presentationResult) << ">, framebufferResized <" << std::format("{}", this->window_->GetFramebufferResized()) << ">" << std::endl;

        this->window_->SetFramebufferResized(false);

        this->RecreateSwapchain();

        // Nothing about the presentation "failed", so the swapchain is recreated and the execution continues with the next frame in flight.
    }
    else if (presentationResult != vk::Result::eSuccess)
    {
        // Technically there shouldn't be other error options and in that case, the present function itself should throw making this redundant.
        throw std::runtime_error("Exception: Presentation was not successful. presentationResult <" + GetPrintableValue(presentationResult) + ">");
    }

    // Timing handling.
    ++this->frameCounter_;
    this->frameEnd_ = std::chrono::high_resolution_clock::now();

    this->PrintLiveFrameInfo(this->internalConfig_.currentFrameInFlightIndex_);

    // First frame flag is set to false.
    currentFrameContext.FirstFrameHandled();
    // Frame in flight is advanced.
    this->AdvanceFrameInFlightIndex();
}

void VulkanVisualizer::ChooseDepthFormat()
{
    this->internalConfig_.depthFormat_ = this->device_->GetSupportedFormat(this->internalConfig_.acceptableDepthFormats_, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);

    if (this->internalConfig_.depthFormat_ == vk::Format::eUndefined)
    {
        throw std::runtime_error("Exception: None of the acceptable formats required for a depth image is supported.");
    }
    else
    {
        LOG_DBG((*this->logger_)) << "Format <" << GetPrintableValue(this->internalConfig_.depthFormat_) << "> was chosen for a depth image." << std::endl;
    }
}

std::array<vk::DescriptorSetLayoutBinding, 1> VulkanVisualizer::PrepareGlobalDataDescriptorSetLayoutBindings() const
{
    vk::DescriptorSetLayoutBinding globalDataDescriptorSetLayoutBinding{};

    globalDataDescriptorSetLayoutBinding
        .setBinding(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    return {globalDataDescriptorSetLayoutBinding};
}

std::unique_ptr<VulkanPipeline> VulkanVisualizer::BuildOpaquePipeline() const
{
    VulkanPipelineBuilder& pipelineBuilder = *this->pipelineBuilder_;

    // Shaders.
    vk::PipelineShaderStageCreateInfo vertexPipelineShaderStageCreateInfo{};

    vertexPipelineShaderStageCreateInfo
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(this->vertexShaderModule_.get()->Get())
        .setPName("main");

    pipelineBuilder.AddPipelineShaderStageCreateInfo(vertexPipelineShaderStageCreateInfo);

    vk::PipelineShaderStageCreateInfo fragmentPipelineShaderStageCreateInfo{};

    fragmentPipelineShaderStageCreateInfo
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(this->fragmentShaderModule_.get()->Get())
        .setPName("main");

    pipelineBuilder.AddPipelineShaderStageCreateInfo(fragmentPipelineShaderStageCreateInfo);

    // Vertex input.
    pipelineBuilder.AddVertexInputBindingDescription(VulkanVertex::GetVertexInputBindingDescription(0));
    pipelineBuilder.AddVertexInputAttributeDescriptions(VulkanVertex::GetVertexInputAttributeDescription(0));

    // Input assembly.
    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};

    pipelineInputAssemblyStateCreateInfo
        .setTopology(vk::PrimitiveTopology::eTriangleList);

    pipelineBuilder.SetPipelineInputAssemblyStateCreateInfo(pipelineInputAssemblyStateCreateInfo);

    // Viewport and scissors.
    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};

    pipelineViewportStateCreateInfo
        .setViewportCount(1)
        .setScissorCount(1);

    pipelineBuilder.SetPipelineViewportStateCreateInfo(pipelineViewportStateCreateInfo);

    // Rasterizer.
    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};

    pipelineRasterizationStateCreateInfo
        .setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setDepthBiasEnable(vk::False)
        .setLineWidth(1.0f);

    pipelineBuilder.SetPipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo);

    // Multisampling.
    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};

    pipelineMultisampleStateCreateInfo
        .setRasterizationSamples(this->visualizerConfig_.pipelineConfig_.rasterizationSamples_.at(VulkanPipelineUseCase::Opaque))
        .setSampleShadingEnable(static_cast<vk::Bool32>(this->visualizerConfig_.pipelineConfig_.sampleShadingEnable_.at(VulkanPipelineUseCase::Opaque)))
        .setMinSampleShading(this->visualizerConfig_.pipelineConfig_.minSampleShading_.at(VulkanPipelineUseCase::Opaque));

    pipelineBuilder.SetPipelineMultisampleStateCreateInfo(pipelineMultisampleStateCreateInfo);

    // Depth.
    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};

    pipelineDepthStencilStateCreateInfo
        .setDepthTestEnable(vk::True)
        .setDepthWriteEnable(vk::True)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(vk::False)
        .setStencilTestEnable(vk::False);

    pipelineBuilder.SetPipelineDepthStencilStateCreateInfo(pipelineDepthStencilStateCreateInfo);

    // Color.
    vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};

    pipelineColorBlendAttachmentState
        .setBlendEnable(vk::False)
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    pipelineBuilder.AddPipelineColorBlendAttachmentState(pipelineColorBlendAttachmentState);

    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};

    pipelineColorBlendStateCreateInfo
        .setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy);

    pipelineBuilder.SetPipelineColorBlendStateCreateInfo(pipelineColorBlendStateCreateInfo);

    // Dynamic states.
    pipelineBuilder.AddDynamicState(vk::DynamicState::eViewport);
    pipelineBuilder.AddDynamicState(vk::DynamicState::eScissor);

    // Dynamic rendering attachments.
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{};

    pipelineRenderingCreateInfo
        .setColorAttachmentFormats(this->swapchain_->GetSurfaceFormat().format)
        .setDepthAttachmentFormat(this->internalConfig_.depthFormat_);

    pipelineBuilder.SetPipelineRenderingCreateInfo(pipelineRenderingCreateInfo);

    // Pipeline layout.
    pipelineBuilder.SetPipelineLayout(*this->pipelineLayout_);
 

    return pipelineBuilder.Build();
}

std::unique_ptr<VulkanPipeline> VulkanVisualizer::BuildTransparentPipeline() const
{
    VulkanPipelineBuilder& pipelineBuilder = *this->pipelineBuilder_;

    // Rasterizer.
    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};

    pipelineRasterizationStateCreateInfo
        .setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        // No culling for the transparent pass.
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setDepthBiasEnable(vk::False)
        .setLineWidth(1.0f);

    pipelineBuilder.SetPipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo);

    // Depth.
    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};

    pipelineDepthStencilStateCreateInfo
        .setDepthTestEnable(vk::True)
        // Depth won't be updated for transparent triangles. All of them will be drawn.
        .setDepthWriteEnable(vk::False)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(vk::False)
        .setStencilTestEnable(vk::False);

    pipelineBuilder.SetPipelineDepthStencilStateCreateInfo(pipelineDepthStencilStateCreateInfo);

    // Color.
    pipelineBuilder.ClearPipelineColorBlendAttachmentStates();

    vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};

    pipelineColorBlendAttachmentState
        // Color blending is enabled.
        .setBlendEnable(vk::True)
        // These define what happens to the color channels.
        // oldRGB = newRGB*newA<eSrcAlpha> +<eAdd> oldRGB*(1-newA)<eOneMinusSrcAlpha>
        // The new(now old) color will have a portion of itself and a portion of the old color.
        // Alpha determines what percentage of a new color will be present.
        .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorBlendOp(vk::BlendOp::eAdd)
        // These define what happens to the alpha channel.
        // oldA = newA*1<eOne> +<eAdd> oldA*0<eZero>
        // The new(now old) alpha will just be the value of the new alpha.
        // It technically doesn't do anything now. oldA in the buffer is never used.
        // The transparencies are baked into the oldRGB.
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd)
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    pipelineBuilder.AddPipelineColorBlendAttachmentState(pipelineColorBlendAttachmentState);

    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};

    pipelineColorBlendStateCreateInfo
        .setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy);

    pipelineBuilder.SetPipelineColorBlendStateCreateInfo(pipelineColorBlendStateCreateInfo);

    return pipelineBuilder.Build();
}

void VulkanVisualizer::HandleEvents()
{
    this->inputHandler_->ResetKeyStates();
    this->inputHandler_->ResetButtonStates();
    this->inputHandler_->ResetScrollDelta();

    this->pollEventsStart_ = std::chrono::high_resolution_clock::now();
    glfwPollEvents();
    this->pollEventsEnd_ = std::chrono::high_resolution_clock::now();

    this->inputHandler_->UpdateMouseState();
}

void VulkanVisualizer::DrawTriangle(const glm::vec3& firstVertexPos, const glm::vec3& secondVertexPos, const glm::vec3& thirdVertexPos, const glm::vec4& color, const glm::vec3& normal)
{
    // Guard against vertex overflow.
    if (this->vertices_.size() + 3 > this->internalConfig_.maxVertexCount_)
    {
        return;
    }

    if (color.a < 1.0f)
    {
        // Transparent triangle as a whole has a separate handling.
        this->StageTransparentTriangle(firstVertexPos, color, normal, secondVertexPos, color, normal, thirdVertexPos, color, normal);
    }
    else
    {
        // Vertices of non-transparent triangles can be pushed in as they are.

        if (this->internalConfig_.useIndexedDraw_ == true)
        {
            AddVertexUnique(this->uniqueVertices_, this->vertices_, this->indexes_, {firstVertexPos, color, normal});
            AddVertexUnique(this->uniqueVertices_, this->vertices_, this->indexes_, {secondVertexPos, color, normal});
            AddVertexUnique(this->uniqueVertices_, this->vertices_, this->indexes_, {thirdVertexPos, color, normal});
        }
        else
        {
            AddVertex(this->vertices_, firstVertexPos, color, normal);
            AddVertex(this->vertices_, secondVertexPos, color, normal);
            AddVertex(this->vertices_, thirdVertexPos, color, normal);
        }
    }
}

void VulkanVisualizer::DrawTriangle(const glm::vec3& firstVertexPos, const glm::vec3& secondVertexPos, const glm::vec3& thirdVertexPos, const glm::vec4& color)
{
    const glm::vec3 normal = glm::cross(secondVertexPos - firstVertexPos, thirdVertexPos - firstVertexPos);

    this->DrawTriangle(firstVertexPos, secondVertexPos, thirdVertexPos, color, normal);
}

void VulkanVisualizer::StageTransparentTriangle(const glm::vec3& firstVertexPos, const glm::vec4& firstVertexColor, const glm::vec3& firstVertexNormal, const glm::vec3& secondVertexPos, const glm::vec4& secondVertexColor, const glm::vec3& secondVertexNormal, const glm::vec3& thirdVertexPos, const glm::vec4& thirdVertexColor, const glm::vec3& thirdVertexNormal)
{
    // Squared distance calculation is postponed till the end of the frame, so the camera can be updated at any time during the frame for convinience.
    const float squaredDistance = 0.0f;
    const size_t newTransparentTriangleIndex = this->transparentTriangleIndexes_.size();

    this->transparentTriangleIndexes_.emplace_back(squaredDistance, newTransparentTriangleIndex);

    if (this->transparentTriangles_.size() > newTransparentTriangleIndex)
    {
        // Data vector has free space. Data are overwritten.

        VulkanVertex& firstTransparentVertex = this->transparentTriangles_[newTransparentTriangleIndex].firstVertex_;
        firstTransparentVertex.position_ = firstVertexPos;
        firstTransparentVertex.color_ = firstVertexColor;
        firstTransparentVertex.normal_ = firstVertexNormal;

        VulkanVertex& secondTransparentVertex = this->transparentTriangles_[newTransparentTriangleIndex].secondVertex_;
        secondTransparentVertex.position_ = secondVertexPos;
        secondTransparentVertex.color_ = secondVertexColor;
        secondTransparentVertex.normal_ = secondVertexNormal;

        VulkanVertex& thirdTransparentVertex = this->transparentTriangles_[newTransparentTriangleIndex].thirdVertex_;
        thirdTransparentVertex.position_ = thirdVertexPos;
        thirdTransparentVertex.color_ = thirdVertexColor;
        thirdTransparentVertex.normal_ = thirdVertexNormal;

        this->transparentTriangles_[newTransparentTriangleIndex].barycenter_ = (firstTransparentVertex.position_ + secondTransparentVertex.position_ + thirdTransparentVertex.position_)/3.0f;
    }
    else
    {
        // Data vector does not have free space. New element is allocated.

        this->transparentTriangles_.emplace_back(VulkanVertex(firstVertexPos, firstVertexColor, firstVertexNormal), VulkanVertex(secondVertexPos, secondVertexColor, secondVertexNormal), VulkanVertex(thirdVertexPos, thirdVertexColor, thirdVertexNormal));
    }
}

void VulkanVisualizer::ProcessTransparentTriangles()
{
    // Counts of opaque vertices and indexes are stored.
    this->opaqueVerticesCount_ = this->vertices_.size();
    this->opaqueIndexesCount_ = this->indexes_.size();

    if (this->internalConfig_.is2D_ == true)
    {
        // Triangles in 2D mode (orthogonal projection is used) are sorted using their Z coordinate.

        for (std::pair<float, size_t>& transparentTriangleIndex : this->transparentTriangleIndexes_)
        {
            transparentTriangleIndex.first = this->transparentTriangles_[transparentTriangleIndex.second].firstVertex_.position_.z;
        }
    }
    else
    {
        // Transparent triangles are sorted from the furthest to the closest to the camera.

        // Squared distance from a barycenter to the camera is filled out.
        const glm::vec3& cameraPosition = this->camera_->GetPosition();

        for (std::pair<float, size_t>& transparentTriangleIndex : this->transparentTriangleIndexes_)
        {
            const glm::vec3 barycenterCameraVector = cameraPosition - this->transparentTriangles_[transparentTriangleIndex.second].barycenter_;
            transparentTriangleIndex.first = glm::dot(barycenterCameraVector, barycenterCameraVector);
        }
    }

    std::ranges::sort(this->transparentTriangleIndexes_, [](const std::pair<float, size_t>& firstTriangleIndex, const std::pair<float, size_t>& secondTriangleIndex){return firstTriangleIndex.first > secondTriangleIndex.first;});

    // Finally, their vertices are pushed into a vector behind the non-transparent vertices.
    for (std::pair<float, size_t>& transparentTriangleIndex : this->transparentTriangleIndexes_)
    {
        const VulkanTransparentTriangle& transparentTriangle = this->transparentTriangles_[transparentTriangleIndex.second];

        if (this->internalConfig_.useIndexedDraw_ == true)
        {
            AddVertexUnique(this->uniqueVertices_, this->vertices_, this->indexes_, transparentTriangle.firstVertex_);
            AddVertexUnique(this->uniqueVertices_, this->vertices_, this->indexes_, transparentTriangle.secondVertex_);
            AddVertexUnique(this->uniqueVertices_, this->vertices_, this->indexes_, transparentTriangle.thirdVertex_);
        }
        else
        {
            AddVertex(this->vertices_, transparentTriangle.firstVertex_);
            AddVertex(this->vertices_, transparentTriangle.secondVertex_);
            AddVertex(this->vertices_, transparentTriangle.thirdVertex_);
        }
    }
}

size_t VulkanVisualizer::GetVerticesByteSize()
{
    return this->vertices_.size()*sizeof(this->vertices_[0]);
}

size_t VulkanVisualizer::GetIndexesByteSize()
{
    return this->indexes_.size()*sizeof(this->indexes_[0]);
}

void VulkanVisualizer::PrepareStagingBuffer(VulkanTransferFrameContext& transferFrameContext)
{
    VulkanBuffer& stagingBuffer = transferFrameContext.GetStagingBuffer();

    // Just a byte buffer, no alignment.
    stagingBuffer.WriteData<VulkanVertex>(this->vertices_, 0);
    stagingBuffer.WriteData<uint32_t>(this->indexes_, this->GetVerticesByteSize());
}

void VulkanVisualizer::RecordCopyCommands(VulkanFrameContext& frameContext, vk::CommandBuffer commandBuffer)
{
    VulkanTransferFrameContext& transferFrameContext = frameContext.GetTransferFrameContext();
    VulkanGraphicsFrameContext& graphicsFrameContext = frameContext.GetGraphicsFrameContext();


    // Vertex buffer copy.
    if (this->GetVerticesByteSize() > 0)
    {
        vk::BufferCopy vertexBufferCopy{};

        vertexBufferCopy
            .setSrcOffset(0)
            .setDstOffset(0)
            .setSize(this->GetVerticesByteSize());

        commandBuffer.copyBuffer(transferFrameContext.GetStagingBuffer().Get(), graphicsFrameContext.GetVertexBuffer().Get(), vertexBufferCopy);
    }


    // Index buffer copy.
    if (this->GetIndexesByteSize() > 0)
    {
        vk::BufferCopy indexBufferCopy{};

        indexBufferCopy
            .setSrcOffset(this->GetVerticesByteSize())
            .setDstOffset(0)
            .setSize(this->GetIndexesByteSize());

        commandBuffer.copyBuffer(transferFrameContext.GetStagingBuffer().Get(), graphicsFrameContext.GetIndexBuffer().Get(), indexBufferCopy);
    }
}

void VulkanVisualizer::RecordTransferCommands(VulkanFrameContext& frameContext)
{
    VulkanTransferFrameContext& transferFrameContext = frameContext.GetTransferFrameContext();
    VulkanGraphicsFrameContext& graphicsFrameContext = frameContext.GetGraphicsFrameContext();

    vk::CommandBuffer transferCommandBuffer = transferFrameContext.GetCommandPool().GetPrimaryCommandBuffer();


    // Begin.
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};

    commandBufferBeginInfo
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    transferCommandBuffer.begin(commandBufferBeginInfo);


    // Vertex and index buffer acquire. Only needed after a first frame.
    if (frameContext.HandlingFirstFrame() == false)
    {
        std::array<vk::BufferMemoryBarrier2, 2> acquireBufferMemoryBarriers{};

        // Acquire from the graphics queue.
        VulkanBufferState srcBufferState(vk::PipelineStageFlagBits2::eNone, vk::AccessFlagBits2::eNone, this->device_->GetGraphicsQueueFamilyIndex());
        // Transfer writes into vertex and index buffer.
        VulkanBufferState dstBufferState(vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite, this->device_->GetTransferQueueFamilyIndex());

        acquireBufferMemoryBarriers[0] = SetupBufferMemoryBarrier(graphicsFrameContext.GetVertexBuffer().Get(), srcBufferState, dstBufferState, vk::WholeSize, 0);
        acquireBufferMemoryBarriers[1] = SetupBufferMemoryBarrier(graphicsFrameContext.GetIndexBuffer().Get(), srcBufferState, dstBufferState, vk::WholeSize, 0);

        vk::DependencyInfo acquireDependencyInfo{};

        acquireDependencyInfo
            .setBufferMemoryBarriers(acquireBufferMemoryBarriers);

        transferCommandBuffer.pipelineBarrier2(acquireDependencyInfo);
    }


    // Copy commands recording.
    this->RecordCopyCommands(frameContext, transferCommandBuffer);


    // Vertex and index buffer release.
    std::array<vk::BufferMemoryBarrier2, 2> releaseBufferMemoryBarriers{};

    // Transfer writes into vertex and index buffers.
    VulkanBufferState srcBufferState(vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite, this->device_->GetTransferQueueFamilyIndex());
    // Release to the graphics queue. Semaphore will be used to synchronize queue submissions. Graphics acquire barrier will mirror this setup.
    VulkanBufferState dstBufferState(vk::PipelineStageFlagBits2::eNone, vk::AccessFlagBits2::eNone, this->device_->GetGraphicsQueueFamilyIndex());

    releaseBufferMemoryBarriers[0] = SetupBufferMemoryBarrier(graphicsFrameContext.GetVertexBuffer().Get(), srcBufferState, dstBufferState, vk::WholeSize, 0);
    releaseBufferMemoryBarriers[1] = SetupBufferMemoryBarrier(graphicsFrameContext.GetIndexBuffer().Get(), srcBufferState, dstBufferState, vk::WholeSize, 0);

    vk::DependencyInfo releaseDependencyInfo{};
   
    releaseDependencyInfo
        .setBufferMemoryBarriers(releaseBufferMemoryBarriers);

    transferCommandBuffer.pipelineBarrier2(releaseDependencyInfo);


    // End.
    transferCommandBuffer.end();
}

void VulkanVisualizer::HandleTransfer(VulkanFrameContext& frameContext)
{
    frameContext.GetTransferFrameContext().GetCommandPool().Reset();


    this->PrepareStagingBuffer(frameContext.GetTransferFrameContext());


    this->RecordTransferCommands(frameContext);


    // Transfer finished semaphore is signaled only when vk::PipelineStageFlagBits2::eTransfer is done.
    std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, 1> signalSemaphores{{{frameContext.GetTransferFinishedSemaphore(), vk::PipelineStageFlagBits2::eTransfer}}};

    // Transfer waits on the graphics finished semaphore. Execute vk::PipelineStageFlagBits2::eTransfer only when the semaphore has been signaled. First frame is excluded.
    if (frameContext.HandlingFirstFrame() == true)
    {
        std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, 0> waitSemaphores{};

        this->SubmitTransfer(frameContext.GetTransferFrameContext(), waitSemaphores, signalSemaphores, this->device_->GetTransferQueue());
    }
    else
    {
        std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, 1> waitSemaphores{{{frameContext.GetGraphicsFinishedSemaphore(), vk::PipelineStageFlagBits2::eTransfer}}};

        this->SubmitTransfer(frameContext.GetTransferFrameContext(), waitSemaphores, signalSemaphores, this->device_->GetTransferQueue());
    }
}

void VulkanVisualizer::UpdateUniformBuffer(VulkanGraphicsFrameContext& graphicsFrameContext)
{
    this->globalData_.cameraView_ = this->camera_->GetViewMatrix();
    this->globalData_.cameraProjection_ = this->internalConfig_.is2D_ == false ? this->camera_->GetPerspectiveProjectionMatrix() : this->camera_->GetOrthographicProjectionMatrix();
    this->globalData_.cameraPosition_ = {this->camera_->GetPosition(), 0.0f};

    this->globalData_.lightSourcePosition_ = this->internalConfig_.lightSourcePosition_;
    this->globalData_.lightSourceColor_ = this->internalConfig_.lightSourceColor_;
    
    this->globalData_.phongParameters_ = this->internalConfig_.phongParameters_;

    VulkanBuffer& uniformBuffer = graphicsFrameContext.GetUniformBuffer();
    
    uniformBuffer.WriteData<VulkanGlobalData>(std::span<const VulkanGlobalData>(&this->globalData_, 1), 0);
}

void VulkanVisualizer::RecordRenderingCommands(VulkanGraphicsFrameContext& graphicsFrameContext, uint32_t swapchainImageIndex, vk::CommandBuffer commandBuffer)
{
    // Depth image barrier.
    // No previous work (depth image per frame in flight). Every pipeline before barrier must reach vk::PipelineStageFlagBits2::eTopOfPipe, no operation to wait on.
    // That happens immediately. Also, there is no previous pipeline is this batch.
    VulkanImageState srcDepthImageState(vk::PipelineStageFlagBits2::eTopOfPipe, {}, vk::ImageLayout::eUndefined);
    // All later pipelines can then proceed with vk::AccessFlagBits2::eDepthStencilAttachmentWrite in stages
    // vk::PipelineStageFlagBits2::eEarlyFragmentTests and vk::PipelineStageFlagBits2::eLateFragmentTests.
    // When they start executing, the image will be in the layout vk::ImageLayout::eDepthAttachmentOptimal.
    VulkanImageState dstDepthImageState(vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests, vk::AccessFlagBits2::eDepthStencilAttachmentWrite, vk::ImageLayout::eDepthAttachmentOptimal);

    vk::ImageSubresourceRange depthImageSubresourceRange{};

    depthImageSubresourceRange
        .setAspectMask(vk::ImageAspectFlagBits::eDepth)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    vk::ImageMemoryBarrier2 depthImageMemoryBarrier = SetupImageMemoryBarrier(graphicsFrameContext.GetDepthImage().Get(), srcDepthImageState, dstDepthImageState, depthImageSubresourceRange);

    // MSAA image barrier.
    if (this->internalConfig_.useMSAA_ == true)
    {
        // No previous work (MSAA image per frame in flight). Every pipeline before barrier must reach vk::PipelineStageFlagBits2::eTopOfPipe, no operation to wait on.
        // That happens immediately. Also, there is no previous pipeline is this batch.
        VulkanImageState srcMSAAImageState(vk::PipelineStageFlagBits2::eTopOfPipe, {}, vk::ImageLayout::eUndefined);
        // All later pipelines can then proceed with vk::AccessFlagBits2::eColorAttachmentWrite in stage vk::PipelineStageFlagBits2::eColorAttachmentOutput.
        // When they start executing, the image will be in the layout vk::ImageLayout::eColorAttachmentOptimal.
        VulkanImageState dstMSAAImageState(vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite, vk::ImageLayout::eColorAttachmentOptimal);

        vk::ImageSubresourceRange msaaImageSubresourceRange{};

        msaaImageSubresourceRange
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1);

        vk::ImageMemoryBarrier2 msaaImageMemoryBarrier = SetupImageMemoryBarrier(graphicsFrameContext.GetMSAAImage().Get(), srcMSAAImageState, dstMSAAImageState, msaaImageSubresourceRange);

        // Recording of the defined barriers.
        std::array<vk::ImageMemoryBarrier2, 2> imageMemoryBarriers = {depthImageMemoryBarrier, msaaImageMemoryBarrier};

        vk::DependencyInfo dependencyInfo{};

        dependencyInfo
            .setDependencyFlags({})
            .setImageMemoryBarriers(imageMemoryBarriers);

        commandBuffer.pipelineBarrier2(dependencyInfo);
    }
    else
    {
        // Recording of the defined barriers.
        std::array<vk::ImageMemoryBarrier2, 1> imageMemoryBarriers = {depthImageMemoryBarrier};

        vk::DependencyInfo dependencyInfo{};

        dependencyInfo
            .setDependencyFlags({})
            .setImageMemoryBarriers(imageMemoryBarriers);

        commandBuffer.pipelineBarrier2(dependencyInfo);
    }


    // Attachment infos.
    // Color.
    vk::ClearColorValue clearColorValue{};

    clearColorValue
        .setFloat32({this->internalConfig_.backgroundColor_.r, this->internalConfig_.backgroundColor_.g, this->internalConfig_.backgroundColor_.b, this->internalConfig_.backgroundColor_.a});

    vk::ClearValue colorClearValue{};

    colorClearValue
        .setColor(clearColorValue);

    vk::RenderingAttachmentInfo colorRenderingAttachmentInfo{};

    if (this->internalConfig_.useMSAA_ == false)
    {
        colorRenderingAttachmentInfo
            .setImageView(this->swapchain_->GetSwapchainImageView(swapchainImageIndex))
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(clearColorValue);
    }
    else
    {
        colorRenderingAttachmentInfo
            // Render target is the MSAA image.
            .setImageView(graphicsFrameContext.GetMSAAImage().GetImageViews().front().get())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(clearColorValue)
            // Samples from the MSAA image are averaged and outputted into the swapchain image.
            .setResolveMode(vk::ResolveModeFlagBits::eAverage)
            .setResolveImageView(this->swapchain_->GetSwapchainImageView(swapchainImageIndex))
            .setResolveImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
    }

    // Depth.
    vk::ClearDepthStencilValue clearDepthStencilValue{};

    clearDepthStencilValue
        .setDepth(1.0f)
        .setStencil(0);

    vk::ClearValue depthClearValue{};

    depthClearValue
        .setDepthStencil(clearDepthStencilValue);

    vk::RenderingAttachmentInfo depthRenderingAttachmentInfo{};

    depthRenderingAttachmentInfo
        .setImageView(graphicsFrameContext.GetDepthImage().GetImageViews().front().get())
        .setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setClearValue(clearDepthStencilValue);


    // Rendering info.
    vk::RenderingInfo renderingInfo{};
 
    vk::Extent2D swapchainExtent = this->swapchain_->GetExtent();

    renderingInfo
        .setRenderArea({{0, 0}, swapchainExtent})
        .setLayerCount(1)
        .setColorAttachments(colorRenderingAttachmentInfo)
        .setPDepthAttachment(&depthRenderingAttachmentInfo);


    commandBuffer.beginRendering(renderingInfo);

    // Opaque first.

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, this->opaquePipeline_->Get());

    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, this->pipelineLayout_->Get(), 0, graphicsFrameContext.GetDescriptorSetsHandles(), {});

    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));

    if (this->internalConfig_.useIndexedDraw_ == true)
    {
        // Binding 0 of the vertex input comes from this buffer.
        // Buffer.
        // Offset is 0. Buffer is read from the beginning.
        commandBuffer.bindVertexBuffers(0, graphicsFrameContext.GetVertexBuffer().Get(), vk::DeviceSize{0});
        // Buffer.
        // Offset is 0. Buffer is read from the beginning.
        // Index type is deduced.
        commandBuffer.bindIndexBuffer(graphicsFrameContext.GetIndexBuffer().Get(), vk::DeviceSize{0}, vk::IndexTypeValue<decltype(this->indexes_)::value_type>::value);
        // Number of vertices/indexes to draw.
        // No instanced rendering -> 1.
        // ...
        commandBuffer.drawIndexed(static_cast<uint32_t>(this->opaqueIndexesCount_), 1, 0, 0, 0);
    }
    else
    {
        commandBuffer.bindVertexBuffers(0, graphicsFrameContext.GetVertexBuffer().Get(), vk::DeviceSize{0});

        commandBuffer.draw(static_cast<uint32_t>(this->opaqueVerticesCount_), 1, 0, 0);
    }

    // Transparent second.

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, this->transparentPipeline_->Get());

    if (this->internalConfig_.useIndexedDraw_ == true)
    {
        commandBuffer.drawIndexed(static_cast<uint32_t>(indexes_.size() - this->opaqueIndexesCount_), 1, static_cast<uint32_t>(opaqueIndexesCount_), 0, 0);
    }
    else
    {
        commandBuffer.draw(static_cast<uint32_t>(this->vertices_.size() - this->opaqueVerticesCount_), 1, static_cast<uint32_t>(this->opaqueVerticesCount_), 0);
    }


    commandBuffer.endRendering();
}

void VulkanVisualizer::RecordGraphicsCommands(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex)
{
    VulkanGraphicsFrameContext& graphicsFrameContext = frameContext.GetGraphicsFrameContext();

    vk::CommandBuffer graphicsCommandBuffer = graphicsFrameContext.GetCommandPool().GetPrimaryCommandBuffer();


    // Begin.
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};

    commandBufferBeginInfo
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    
    graphicsCommandBuffer.begin(commandBufferBeginInfo);


    // Vertex and index buffer acquire
    std::array<vk::BufferMemoryBarrier2, 2> acquireBufferMemoryBarriers{};

    // Acquire from the transfer queue.
    VulkanBufferState srcBufferState(vk::PipelineStageFlagBits2::eNone, vk::AccessFlagBits2::eNone, this->device_->GetTransferQueueFamilyIndex());
    // Graphics reads from vertex buffer.
    VulkanBufferState dstBufferState_Vertex(vk::PipelineStageFlagBits2::eVertexInput, vk::AccessFlagBits2::eVertexAttributeRead, this->device_->GetGraphicsQueueFamilyIndex());
    // Graphics reads from index buffer.
    VulkanBufferState dstBufferState_Index(vk::PipelineStageFlagBits2::eVertexInput, vk::AccessFlagBits2::eIndexRead, this->device_->GetGraphicsQueueFamilyIndex());

    acquireBufferMemoryBarriers[0] = SetupBufferMemoryBarrier(graphicsFrameContext.GetVertexBuffer().Get(), srcBufferState, dstBufferState_Vertex, vk::WholeSize, 0);
    acquireBufferMemoryBarriers[1] = SetupBufferMemoryBarrier(graphicsFrameContext.GetIndexBuffer().Get(), srcBufferState, dstBufferState_Index, vk::WholeSize, 0);

    vk::DependencyInfo acquireDependencyInfo{};

    acquireDependencyInfo
        .setBufferMemoryBarriers(acquireBufferMemoryBarriers);

    graphicsCommandBuffer.pipelineBarrier2(acquireDependencyInfo);


    // Swapchain image is transitioned to a draw target state.
    // By definition (even in the first frame), the "presentation stage" when the image can be used
    // after presentation is vk::PipelineStageFlagBits2::eColorAttachmentOutput.
    VulkanImageState srcImageState(vk::PipelineStageFlagBits2::eColorAttachmentOutput, {}, vk::ImageLayout::eUndefined);
    VulkanImageState dstImageState(vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite, vk::ImageLayout::eColorAttachmentOptimal);

    this->swapchain_->InsertSwapchainImageBarrier(graphicsCommandBuffer, swapchainImageIndex, srcImageState, dstImageState);


    // Graphics commands recording.
    this->RecordRenderingCommands(graphicsFrameContext, swapchainImageIndex, graphicsCommandBuffer);


    // Swapchain image is transitioned to a present state.
    // By definition, the vk::PipelineStageFlagBits2::eBottomOfPipe is set.
    // The consumer is not a Vulkan pipeline stage, but a presentation engine. So the closest synchronization boundary is used.
    srcImageState = VulkanImageState(vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite, vk::ImageLayout::eColorAttachmentOptimal);
    dstImageState = VulkanImageState(vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eNone, vk::ImageLayout::ePresentSrcKHR);

    this->swapchain_->InsertSwapchainImageBarrier(graphicsCommandBuffer, swapchainImageIndex, srcImageState, dstImageState);


    // Vertex and index buffer release.
    std::array<vk::BufferMemoryBarrier2, 2> releaseBufferMemoryBarriers{};

    // Graphics reads from vertex buffer.
    VulkanBufferState srcBufferState_Vertex(vk::PipelineStageFlagBits2::eVertexInput, vk::AccessFlagBits2::eVertexAttributeRead, this->device_->GetGraphicsQueueFamilyIndex());
    // Graphics reads from index buffer.
    VulkanBufferState srcBufferState_Index(vk::PipelineStageFlagBits2::eVertexInput, vk::AccessFlagBits2::eIndexRead, this->device_->GetGraphicsQueueFamilyIndex());
    // Release to the transfer queue. Semaphore will be used to synchronize queue submissions. Transfer acquire barrier will mirror this setup.
    VulkanBufferState dstBufferState(vk::PipelineStageFlagBits2::eNone, vk::AccessFlagBits2::eNone, this->device_->GetTransferQueueFamilyIndex());

    releaseBufferMemoryBarriers[0] = SetupBufferMemoryBarrier(graphicsFrameContext.GetVertexBuffer().Get(), srcBufferState_Vertex, dstBufferState, vk::WholeSize, 0);
    releaseBufferMemoryBarriers[1] = SetupBufferMemoryBarrier(graphicsFrameContext.GetIndexBuffer().Get(), srcBufferState_Index, dstBufferState, vk::WholeSize, 0);

    vk::DependencyInfo releaseDependencyInfo{};

    releaseDependencyInfo
        .setBufferMemoryBarriers(releaseBufferMemoryBarriers);

    graphicsCommandBuffer.pipelineBarrier2(releaseDependencyInfo);


    // End.
    graphicsCommandBuffer.end();
}

void VulkanVisualizer::HandleGraphics(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex)
{
    frameContext.GetGraphicsFrameContext().GetCommandPool().Reset();


    this->UpdateUniformBuffer(frameContext.GetGraphicsFrameContext());


    this->RecordGraphicsCommands(frameContext, swapchainImageIndex);


    // 1) Graphics commands can be executed upPoint to the vk::PipelineStageFlagBits2::eColorAttachmentOutput stage. After that, the swapchain image must be available.
    // 2) Graphics waits on the transfer finished semaphore. Execute vk::PipelineStageFlagBits2::eVertexInput only when the semaphore has been signaled.
    std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, 2> waitSemaphores{{{frameContext.GetImageAvailableSemaphore(), vk::PipelineStageFlagBits2::eColorAttachmentOutput}, {frameContext.GetTransferFinishedSemaphore(), vk::PipelineStageFlagBits2::eVertexInput}}};

    // 1) All graphics stages have to be complete before the semaphore is signaled.
    // 2) Graphics finished semaphore is signaled only when vk::PipelineStageFlagBits2::eAllGraphics is done.
    std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, 2> signalSemaphores{{{this->swapchain_->GetSwapchainImageRFSemaphore(swapchainImageIndex), vk::PipelineStageFlagBits2::eAllGraphics}, {frameContext.GetGraphicsFinishedSemaphore(), vk::PipelineStageFlagBits2::eAllGraphics}}};

    this->SubmitGraphics(frameContext.GetGraphicsFrameContext(), waitSemaphores, signalSemaphores, frameContext.GetFrameInFlightFence(), this->device_->GetGraphicsQueue());
}

void VulkanVisualizer::RecordTransferAndGraphicsCommands(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex)
{
    VulkanGraphicsFrameContext& graphicsFrameContext = frameContext.GetGraphicsFrameContext();

    vk::CommandBuffer graphicsCommandBuffer = graphicsFrameContext.GetCommandPool().GetPrimaryCommandBuffer();


    // Begin.
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};

    commandBufferBeginInfo
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    graphicsCommandBuffer.begin(commandBufferBeginInfo);


    // Copy commands recording.
    this->RecordCopyCommands(frameContext, graphicsCommandBuffer);


    // Vertex and index buffer barriers.
    std::array<vk::BufferMemoryBarrier2, 2> bufferMemoryBarriers{};

    // Data are copied into vertex and index buffers.
    VulkanBufferState srcBufferState(vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite);
    // Read from vertex buffer.
    VulkanBufferState dstBufferState_Vertex(vk::PipelineStageFlagBits2::eVertexInput, vk::AccessFlagBits2::eVertexAttributeRead);
    // Read from index buffer.
    VulkanBufferState dstBufferState_Index(vk::PipelineStageFlagBits2::eVertexInput, vk::AccessFlagBits2::eIndexRead);

    bufferMemoryBarriers[0] = SetupBufferMemoryBarrier(graphicsFrameContext.GetVertexBuffer().Get(), srcBufferState, dstBufferState_Vertex, vk::WholeSize, 0);
    bufferMemoryBarriers[1] = SetupBufferMemoryBarrier(graphicsFrameContext.GetIndexBuffer().Get(), srcBufferState, dstBufferState_Index, vk::WholeSize, 0);

    vk::DependencyInfo releaseDependencyInfo{};

    releaseDependencyInfo
        .setBufferMemoryBarriers(bufferMemoryBarriers);

    graphicsCommandBuffer.pipelineBarrier2(releaseDependencyInfo);


    // Swapchain image is transitioned to a draw target state.
    // By definition (even in the first frame), the "presentation stage" when the image can be used
    // after presentation is vk::PipelineStageFlagBits2::eColorAttachmentOutput.
    VulkanImageState srcImageState(vk::PipelineStageFlagBits2::eColorAttachmentOutput, {}, vk::ImageLayout::eUndefined);
    VulkanImageState dstImageState(vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite, vk::ImageLayout::eColorAttachmentOptimal);

    this->swapchain_->InsertSwapchainImageBarrier(graphicsCommandBuffer, swapchainImageIndex, srcImageState, dstImageState);


    // Graphics commands recording.
    this->RecordRenderingCommands(graphicsFrameContext, swapchainImageIndex, graphicsCommandBuffer);


    // Swapchain image is transitioned to a present state.
    // By definition, the vk::PipelineStageFlagBits2::eBottomOfPipe is set.
    // The consumer is not a Vulkan pipeline stage, but a presentation engine. So the closest synchronization boundary is used.
    srcImageState = VulkanImageState(vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite, vk::ImageLayout::eColorAttachmentOptimal);
    dstImageState = VulkanImageState(vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eNone, vk::ImageLayout::ePresentSrcKHR);

    this->swapchain_->InsertSwapchainImageBarrier(graphicsCommandBuffer, swapchainImageIndex, srcImageState, dstImageState);


    // End.
    graphicsCommandBuffer.end();
}

void VulkanVisualizer::HandleTransferAndGraphics(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex)
{
    // Transfer commands will be executed by the graphics queue.

    frameContext.GetGraphicsFrameContext().GetCommandPool().Reset();


    this->PrepareStagingBuffer(frameContext.GetTransferFrameContext());


    this->UpdateUniformBuffer(frameContext.GetGraphicsFrameContext());


    this->RecordTransferAndGraphicsCommands(frameContext, swapchainImageIndex);

    // 1) Graphics commands can be executed upPoint to the vk::PipelineStageFlagBits2::eColorAttachmentOutput stage. After that, the swapchain image must be available.
    std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, 1> waitSemaphores{{{frameContext.GetImageAvailableSemaphore(), vk::PipelineStageFlagBits2::eColorAttachmentOutput}}};

    // 1) All graphics stages have to be complete before the semaphore is signaled.
    std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, 1> signalSemaphores{{{this->swapchain_->GetSwapchainImageRFSemaphore(swapchainImageIndex), vk::PipelineStageFlagBits2::eAllGraphics}}};

    this->SubmitGraphics(frameContext.GetGraphicsFrameContext(), waitSemaphores, signalSemaphores, frameContext.GetFrameInFlightFence(), this->device_->GetGraphicsQueue());
}

void VulkanVisualizer::RecreateSwapchain()
{
    LOG_DBG((*this->logger_)) << "Swapchain recreation initiated." << std::endl;

    // Minimalization is a case that is handled separately.
    this->window_->HandleMinimization();

    // Device must be idle.
    this->device_->WaitIdle();

    // Physical device must support the new requirements.
    this->device_->RecheckSurfaceRelatedRequirements();

    // New swapchain is constructed (old swapchain is provided to the constructor).
    std::unique_ptr<VulkanSwapchain> newSwapchain = std::make_unique<VulkanSwapchain>(*this->window_, *this->surface_, *this->device_, this->visualizerConfig_, *this->swapchain_);

    // New swapchain replaces the old one.
    this->swapchain_ = std::move(newSwapchain);

    // Viewport dimensions (and aspect ratio) could have changed, so the camera is updated.
    this->camera_->SetViewportDimensions(this->swapchain_->GetExtent().width, this->swapchain_->GetExtent().height);

    // Recreation of depth and MSAA images.
    for (std::unique_ptr<VulkanFrameContext>& frameContext : this->frameContexts_)
    {
        frameContext->GetGraphicsFrameContext().RecreateDepthImage(*this->swapchain_, this->internalConfig_, this->visualizerConfig_);
        frameContext->GetGraphicsFrameContext().RecreateMSAAImage(*this->swapchain_, this->visualizerConfig_);
    }

    LOG_DBG((*this->logger_)) << "Swapchain recreation completed." << std::endl;
}

void VulkanVisualizer::AdvanceFrameInFlightIndex()
{
    this->internalConfig_.currentFrameInFlightIndex_ = (this->internalConfig_.currentFrameInFlightIndex_ + 1) % this->internalConfig_.framesInFlightCount_;
}

void VulkanVisualizer::UpdateTimingVariables()
{
    if (this->frameCounter_ == 0)
    {
        // Results in delta time of zero in the first frame.
        this->frameStart_ = std::chrono::high_resolution_clock::now();
        this->lastFrameStart_ = this->frameStart_;
    }
    else
    {
        this->lastFrameStart_ = this->frameStart_;
        this->frameStart_ = std::chrono::high_resolution_clock::now();
    }

    this->deltaTime_ = this->frameStart_ - this->lastFrameStart_;
    this->simulationTime_ += this->deltaTime_;

    //  10 (arbitrarily chosen) first frames are discarded so the initial variance does not skew the data.
    if (this->frameCounter_ > 10)
    {
        this->accumulatedFramesTime_ += this->deltaTime_;
        ++this->accumulatedFramesCount_;
    }
}

// Prints timing info (start of frame).
void VulkanVisualizer::PrintLiveTimingInfo() const
{
    if (this->internalConfig_.showLiveTimingInfo_ == true)
    {
        LOG_INF((*this->logger_))
            << "DT: " << std::format("{:6.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->deltaTime_).count()) << "ms, "
            << "ST: " << std::format("{:6.2f}", std::chrono::duration_cast<std::chrono::duration<double>>(this->simulationTime_).count()) << "s "
            << "fps: " << std::format("{:6.2f}", 1.0/std::chrono::duration_cast<std::chrono::duration<double>>(this->deltaTime_).count()) << "\n";
    }
}

void VulkanVisualizer::PrintLiveFrameInfo(size_t currentFrameInFlightIndex) const
{
    if (this->internalConfig_.showLiveFrameInfo_ == true)
    {
        LOG_INF((*this->logger_)) << "Frame: " << this->frameCounter_ << "[" << currentFrameInFlightIndex << "], "
            << "S-PES: " << std::format("{:7.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->pollEventsStart_ - this->frameStart_).count()) << "ms, "
            << "PES-PEE: " << std::format("{:7.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->pollEventsEnd_ - this->pollEventsStart_).count()) << "ms, "
            << "PEE-UWS: " << std::format("{:7.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->userWorkloadStart_ - this->pollEventsEnd_).count()) << "ms, "
            << "UWS-UWE: " << std::format("{:7.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->userWorkloadEnd_ - this->userWorkloadStart_).count()) << "ms, "
            << "UWE-BF: " << std::format("{:7.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->frameBeforeFence_ - this->userWorkloadEnd_).count()) << "ms, "
            << "BF-FT: " << std::format("{:7.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->frameFenceThrough_ - this->frameBeforeFence_).count()) << "ms, "
            << "FT-E: " << std::format("{:7.3f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->frameEnd_ - this->frameFenceThrough_).count()) << "ms" << "\n";
    }
}

void VulkanVisualizer::PrintPostCloseTimingInfo() const
{
    if (this->internalConfig_.showPostCloseTimingInfo_ == true)
    {
        LOG_INF((*this->logger_))
            << "Accumulated frames: " << this->accumulatedFramesCount_ << ", "
            << "Accumulated time: " << std::format("{:6.2f}", std::chrono::duration_cast<std::chrono::duration<double>>(this->accumulatedFramesTime_).count()) << "s, "
            << "Average frame time: " << std::format("{:6.2f}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->accumulatedFramesTime_).count()/static_cast<double>(this->accumulatedFramesCount_)) << "ms, "
            << "Average fps: " << std::format("{:6.2f}", 1.0/(std::chrono::duration_cast<std::chrono::duration<double>>(this->accumulatedFramesTime_).count()/static_cast<double>(this->accumulatedFramesCount_))) << "\n";
    }
}
