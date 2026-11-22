
#pragma once

#include <vulkan/vulkan.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <chrono>
#include <ranges>

#include "Logger.h"
#include "VulkanConfig.h"
#include "VulkanInternalConfig.h"
#include "VulkanCallbackDispatcher.h"
#include "VulkanWindow.h"
#include "VulkanInputHandler.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanVMA.h"
#include "VulkanSwapchain.h"
#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanPipeline.h"
#include "VulkanFrame.h"
#include "VulkanVertex.h"
#include "VulkanCamera.h"
#include "VulkanGlobalData.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"


class VulkanVisualizer
{
private:

    // Init config.
    VulkanVisualizerConfig visualizerConfig_;

    // Internal runtime config.
    VulkanInternalConfig internalConfig_{};

    // Logging handler.
    std::unique_ptr<Logger> logger_;

    // Window.
    std::unique_ptr<VulkanWindow> window_;

    // Input handler.
    std::unique_ptr<VulkanInputHandler> inputHandler_;

    // GLFW callback dispatcher.
    std::unique_ptr<VulkanCallbackDispatcher> callbackDispatcher_;

    // Instance.
    std::unique_ptr<VulkanInstance> instance_;

    // Surface.
    std::unique_ptr<VulkanSurface> surface_;

    // Device.
    std::unique_ptr<VulkanDevice> device_;

    // Vulkan Memory Allocator.
    std::unique_ptr<VulkanVMA> vma_;

    // Swapchain.
    std::unique_ptr<VulkanSwapchain> swapchain_;

    // Descriptor pool.
    std::unique_ptr<VulkanDescriptorPool> descriptorPool_;

    // Shader modules.
    std::unique_ptr<VulkanShaderModule> vertexShaderModule_;
    std::unique_ptr<VulkanShaderModule> fragmentShaderModule_;

    // Descriptor set layout.
    std::unique_ptr<VulkanDescriptorSetLayout> globalDataDescriptorSetLayout_;
    std::vector<std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts_;

    // Pipeline layout.
    std::unique_ptr<VulkanPipelineLayout> pipelineLayout_;

    // Pipelines.
    std::unique_ptr<VulkanPipelineBuilder> pipelineBuilder_;
    std::unique_ptr<VulkanPipeline> opaquePipeline_;
    std::unique_ptr<VulkanPipeline> transparentPipeline_;

    // Frame contexts.
    std::vector<std::unique_ptr<VulkanFrameContext>> frameContexts_;

    // Vertices (with indexes) to be drawn.
    std::vector<VulkanVertex> vertices_;
    std::vector<uint32_t> indexes_;
    size_t opaqueVerticesCount_ = 0;
    size_t opaqueIndexesCount_ = 0;
    // Possible since C++ 20.
    // Lambda is turned into a nameless functor, that is now default constructible (if it has no captures).
    // This allows it to be instantiated inside of the unordered_map (like Hash hasher = Hash()).
    // More efficient than a parameter lambda, since the calls can be resolved during compile time.
    std::unordered_map<VulkanVertex, uint32_t, decltype([](const VulkanVertex& vertex){return vertex.Hash();})> uniqueVertices_;

    // Vector that stores actual data of transparent triangles to be drawn.
    struct VulkanTransparentTriangle
    {
        VulkanVertex firstVertex_{};
        VulkanVertex secondVertex_{};
        VulkanVertex thirdVertex_{};

        glm::vec3 barycenter_{};

        VulkanTransparentTriangle(const VulkanVertex& firstVertex, const VulkanVertex& secondVertex, const VulkanVertex& thirdVertex) :
            firstVertex_(firstVertex),
            secondVertex_(secondVertex),
            thirdVertex_(thirdVertex),
            barycenter_((firstVertex.position_ + secondVertex.position_ + thirdVertex.position_)/3.0f)
        {
        }
    };
    std::vector<VulkanTransparentTriangle> transparentTriangles_;
    // Vector of distance to camera and triangle index.
    // Used for sorting. Sorting pairs is ~25% faster than sorting only indexes and accessing distance value through them.
    // Indexes are used instead of pointers to avoid their deallocation each frame during clear operation.
    std::vector<std::pair<float, size_t>> transparentTriangleIndexes_;

    // Global data stored in a uniform buffer.
    VulkanGlobalData globalData_{};

    // Camera.
    std::unique_ptr<VulkanCamera> camera_;

    // Timing variables.
    size_t frameCounter_ = 0;
    std::chrono::nanoseconds deltaTime_ = std::chrono::nanoseconds::zero();
    std::chrono::nanoseconds simulationTime_ = std::chrono::nanoseconds::zero();
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameStart_;
    std::chrono::time_point<std::chrono::high_resolution_clock> frameStart_;
    std::chrono::time_point<std::chrono::high_resolution_clock> pollEventsStart_;
    std::chrono::time_point<std::chrono::high_resolution_clock> pollEventsEnd_;
    std::chrono::time_point<std::chrono::high_resolution_clock> userWorkloadStart_;
    std::chrono::time_point<std::chrono::high_resolution_clock> userWorkloadEnd_;
    std::chrono::time_point<std::chrono::high_resolution_clock> frameBeforeFence_;
    std::chrono::time_point<std::chrono::high_resolution_clock> frameFenceThrough_;
    std::chrono::time_point<std::chrono::high_resolution_clock> frameEnd_;
    size_t accumulatedFramesCount_ = 0;
    std::chrono::nanoseconds accumulatedFramesTime_ = std::chrono::nanoseconds::zero();
    
public:

    // Constructor.
    VulkanVisualizer();
    // Destructor.
    ~VulkanVisualizer();

    // Configuration setters (must be used before Init() is called).
    void SetInitialVisualizerMode(bool is2D);
    void SetInitialRendererName(std::string name);
    // If window coordinates or dimensions are empty and window mode is "Window", fallback value of (1280x720) will be used and the window will be centered.
    void SetInitialWindowMode(VulkanWindowMode windowMode, int windowWidth = 0, int windowHeight = 0, int windowX = std::numeric_limits<int>::min(), int windowY = std::numeric_limits<int>::min());
    void SetInitialMaxVertexCount(size_t vertexCount);
    void SetInitialUseMSAA(bool useMSAA, size_t msaaSampleCount = 8);
    void SetInitialShowLiveTimingInfo(bool showLiveFrameInfo);
    void SetInitialShowLiveFrameInfo(bool showLiveFrameInfo);
    void SetInitialShowPostCloseTimingInfo(bool showPostCloseTimingInfo);

    // Full initialization of the renderer.
    void Init();

    // Runtime setters (must be used after Init() is called).

    // Setter of window mode.
    // If window coordinates or dimensions are empty and window mode is "Window", fallback value of (1280x720) will be used and the window will be centered.
    void SetWindowMode(VulkanWindowMode windowMode, int windowWidth = 0, int windowHeight = 0, int windowX = std::numeric_limits<int>::min(), int windowY = std::numeric_limits<int>::min());

    // Setter of a background color.
    void SetBackgroundColor(float colorR, float colorG, float colorB);

    // Setters of camera state components.
    void SetCameraPosition(float positionX, float positionY, float positionZ);
    void SetCameraTargetPosition(float targetPositionX, float targetPositionY, float targetPositionZ);
    void SetCameraUp(float upX, float upY, float upZ);
    void SetCameraUpFallback(float upFallbackX, float upFallbackY, float upFallbackZ);
    // Getters of camera state components.
    void GetCameraPosition(float& positionX, float& positionY, float& positionZ) const;
    void GetCameraTargetPosition(float& targetPositionX, float& targetPositionY, float& targetPositionZ) const;
    void GetCameraUp(float& upX, float& upY, float& upZ) const;
    void GetCameraUpFallback(float& upFallbackX, float& upFallbackY, float& upFallbackZ) const;
    void GetCameraPerspectiveMatrixComponents(float& fieldOfView, float& aspectRatio, float& nearPlane, float& farPlane) const;

    // Setters of light source components.
    void SetLightSourcePosition(float positionX, float positionY, float positionZ);
    void SetLightSourceColor(float colorR, float colorG, float colorB);

    // Setters of parameters of Phong reflection model.
    void SetUsePhongReflectionModel(bool usePhongReflectionModel);
    void SetPhongAmbient(float phongAmbient);
    void SetPhongShininess(float phongShininess);

    // Input setters.
    void SetCursorIsCaptured(bool cursorIsCaptured);
    // Input queries.
    bool IsKeyDown(int glfwKey) const;
    bool IsKeyJustPressed(int glfwKey) const;
    bool IsKeyJustReleased(int glfwKey) const;
    bool IsButtonDown(int glfwButton) const;
    bool IsButtonJustPressed(int glfwButton) const;
    bool IsButtonJustReleased(int glfwButton) const;
    // [0,0] is in the middle of the screen.
    void GetCursorPos(double& cursorX, double& cursorY) const;
    void GetCursorDelta(double& cursorDeltaX, double& cursorDeltaY) const;
    void GetScrollDelta(double& scrollDeltaX, double& scrollDeltaY) const;
    // Input helpers.
    // Designed for 3D mode. Updates free moving camera position.
    // Controls:
    //   Mouse - look around.
    //   WSAD - Move forward/backward, strafe left/right.
    //   SPACE - Move up.
    //   SHIFT - Double camera speed.
    void InputBasedFreeMovementCameraUpdate(float cameraSpeed, float cameraSensitivity);

    // Returns current window size.
    std::pair<int, int> GetWindowSize() const;
    // Returns current framebuffer size.
    std::pair<int, int> GetFramebufferSize() const;

    // 3D API.

    // Draws a raw 3D triangle. It is meant to be used to construct primitives that the API does not explicitly provide.
    void Draw3DTriangle(float firstVertexX, float firstVertexY, float firstVertexZ, float secondVertexX, float secondVertexY, float secondVertexZ, float thirdVertexX, float thirdVertexY, float thirdVertexZ, float colorR, float colorG, float colorB, float colorA);
    // Draws a cube/icosahedron in a fixed orientation. These are meant to be used for representing a 3D point.
    void Draw3DIcosahedron(float centerX, float centerY, float centerZ, float colorR, float colorG, float colorB, float colorA, float diameter);
    void Draw3DCube(float centerX, float centerY, float centerZ, float colorR, float colorG, float colorB, float colorA, float sideLength);
    // Draws a 3D sphere. It is produced by recursively subdividing edges of an icosahedron
    // and projecting the new vertices onto a bounding sphere (SUBDIVISIONS == 0 draws an icosahedron).
    // Subdivision is precomputed at compile time, but even then this is a much more expensive primitive
    // to draw (compared to let's say a cube) just from a triangle count standpoint.
    // It is meant to be used in situations when precise sphere is necessary to be drawn.
    template<size_t SUBDIVISIONS = 2>
    void Draw3DSphere(float centerX, float centerY, float centerZ, float colorR, float colorG, float colorB, float colorA, float diameter);
    // Draws a line segment represented by a cylinder with a given number of sides.
    // Cap offset shifts the start and end of the line segment from the actual start point and end point.
    // Cap width creates a cone at the start and end of the line segment, making them not flat.
    template<size_t SIDES = 4>
    void Draw3DLineSegment(float startX, float startY, float startZ, float endX, float endY, float endZ, float colorR, float colorG, float colorB, float colorA, float thickness, float capOffset = 0.0f, float capWidth = 0.0f);
    // Draws a line consisting of multiple segments in 3D space. Can be closed to form a loop.
    template<size_t SIDES = 6>
    void Draw3DPolyline(std::span<const std::array<float, 3>> points, float colorR, float colorG, float colorB, float colorA, float thickness, bool isLoop = false);
    // Draws a zero width plane in a 3D space. Three points define a four sided parallelogram.
    // Can be subdivided into SUBDIVISIONS*SUBDIVISIONS grid. Each cell consists of two triangles.
    // Designed to counteract artifacts of CPU triangle sorting for transparent planes.
    template<size_t SUBDIVISIONS = 1>
    void Draw3DPlane(float firstPointX, float firstPointY, float firstPointZ, float secondPointX, float secondPointY, float secondPointZ, float thirdPointX, float thirdPointY, float thirdPointZ, float colorR, float colorG, float colorB, float colorA);
    // Draws a zero width disk in a 3D space. Normal points toward a direction point.
    template<size_t SIDES = 16>
    void Draw3DDisk(float centerX, float centerY, float centerZ, float directionX, float directionY, float directionZ, float colorR, float colorG, float colorB, float colorA, float diameter);
    // Draws a 6-sided closed cuboid like shape whose angles do not have to be 90 degrees.
    void Draw3DBox(float originX, float originY, float originZ, float rightX, float rightY, float rightZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ, float colorR, float colorG, float colorB, float colorA);  
    // Axis aligned wrapper around Draw3DBox().
    void Draw3DRectangularCuboid(float originX, float originY, float originZ, float oppositeX, float oppositeY, float oppositeZ, float colorR, float colorG, float colorB, float colorA);
    // Draws a cone in any orientation in 3D space.
    template<size_t SIDES = 16>
    void Draw3DCone(float baseCenterX, float baseCenterY, float baseCenterZ, float tipX, float tipY, float tipZ, float colorR, float colorG, float colorB, float colorA, float baseWidth);
    // Draws an arrow in 3D space. Composite shape consisting of a line segment and a cone.
    template<size_t SIDES = 6>
    void Draw3DArrow(float startX, float startY, float startZ, float endX, float endY, float endZ, float colorR, float colorG, float colorB, float colorA, float thickness);
    // Draws an arrow in the positive direction of world axes. Composite shape consisting of three arrows.
    template<size_t SIDES = 6>
    void Draw3DAxes(float originX, float originY, float originZ, float length, float thickness, float colorA);
    // Draws a box shaped wireframe in 3D space. Composite shape consisting of icosahedrons and line segments.
    template<size_t SIDES = 6>
    void Draw3DWireframe(float originX, float originY, float originZ, float rightX, float rightY, float rightZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ, float colorR, float colorG, float colorB, float colorA, float thickness);
    // Draws an evenly spaced nxm grid in 3D space. Composite shape consisting of icosahedrons and line segments.
    template<size_t SIDES = 6>
    void Draw3DGrid(float originX, float originY, float originZ, float rightX, float rightY, float rightZ, float forwardX, float forwardY, float forwardZ, float colorR, float colorG, float colorB, float colorA, float thickness, size_t n, size_t m);

    // 2D API.

    // Draws a raw 2D triangle. It is meant to be used to construct primitives that the API does not explicitly provide.
    void Draw2DTriangle(float firstVertexX, float firstVertexY, float secondVertexX, float secondVertexY, float thirdVertexX, float thirdVertexY, float Z, float colorR, float colorG, float colorB, float colorA);
    // Draws a 2D point represented by a regular N-gon.
    template<size_t SIDES = 8>
    void Draw2DPoint(float centerX, float centerY, float Z, float colorR, float colorG, float colorB, float colorA, float diameter);
    // Draws a line segment represented by a rectangle.
    // Cap offset shifts the start and end of the line segment from the actual start point and end point.
    void Draw2DLineSegment(float startX, float startY, float endX, float endY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness, float capOffset = 0.0f);
    // Draws a line consisting of multiple segments in 2D space. Can be closed to form a loop.
    void Draw2DPolyline(std::span<const std::array<float, 2>> points, float Z, float colorR, float colorG, float colorB, float colorA, float thickness, bool isLoop = false);
    // Draws a parallelogram in 2D space.
    void Draw2DParallelogram(float originX, float originY, float rightX, float rightY, float forwardX, float forwardY, float Z, float colorR, float colorG, float colorB, float colorA);
    // Axis aligned wrapper around Draw2DParallelogram().
    void Draw2DRectangle(float originX, float originY, float oppositeX, float oppositeY, float Z, float colorR, float colorG, float colorB, float colorA);
    // Draws an arrow in 2D space. Composite shape consisting of a line segment and a triangle.
    void Draw2DArrow(float startX, float startY, float endX, float endY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness);
    // Draws an arrow in the positive direction of world axes. Composite shape consisting of two arrows.
    void Draw2DAxes(float originX, float originY, float Z, float length, float thickness, float colorA); 
    // Draws a box shaped wireframe in 2D space. Composite shape consisting of points and line segments.
    template<size_t SIDES = 8>
    void Draw2DWireframe(float originX, float originY, float rightX, float rightY, float forwardX, float forwardY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness);
    // Draws an evenly spaced nxm grid in 2D space. Composite shape consisting of points and line segments.
    template<size_t SIDES = 8>
    void Draw2DGrid(float originX, float originY, float rightX, float rightY, float forwardX, float forwardY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness, size_t n, size_t m);

    // Handles things necessary to be done at the start of the frame.
    void HandleFrameStart();

    // Check if the window should be closed.
    bool WindowShouldClose() const;
    
    // Drawing of the setup frame.
    void DrawFrame();

    // Returns delta time.
    template<typename T>
    T GetDeltaTime() const;
    // Returns simulation time.
    template<typename T>
    T GetSimulationTime() const;

private:

    void ChooseDepthFormat();

    std::array<vk::DescriptorSetLayoutBinding, 1> PrepareGlobalDataDescriptorSetLayoutBindings() const;

    std::unique_ptr<VulkanPipeline> BuildOpaquePipeline() const;
    // Expects opaque pipeline to be built first.
    std::unique_ptr<VulkanPipeline> BuildTransparentPipeline() const;

    void HandleEvents();

    // Internal functions for drawing a triangle. With or without explicit normal.
    void DrawTriangle(const glm::vec3& firstVertexPos, const glm::vec3& secondVertexPos, const glm::vec3& thirdVertexPos, const glm::vec4& color);
    void DrawTriangle(const glm::vec3& firstVertexPos, const glm::vec3& secondVertexPos, const glm::vec3& thirdVertexPos, const glm::vec4& color, const glm::vec3& normal);

    // Stages a single triangle into a staging vector.
    void StageTransparentTriangle(const glm::vec3& firstVertexPos, const glm::vec4& firstVertexColor, const glm::vec3& firstVertexNormal, const glm::vec3& secondVertexPos, const glm::vec4& secondVertexColor, const glm::vec3& secondVertexNormal, const glm::vec3& thirdVertexPos, const glm::vec4& thirdVertexColor, const glm::vec3& thirdVertexNormal);
    // Sorts all the triangles 
    void ProcessTransparentTriangles();

    size_t GetVerticesByteSize();
    size_t GetIndexesByteSize();

    void PrepareStagingBuffer(VulkanTransferFrameContext& transferFrameContext);
    void RecordCopyCommands(VulkanFrameContext& frameContext, vk::CommandBuffer commandBuffer);
    void RecordTransferCommands(VulkanFrameContext& frameContext);
    template<size_t N, size_t M>
    void SubmitTransfer(VulkanTransferFrameContext& transferFrameContext, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, N>& waitSemaphores, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, M>& signalSemaphores, vk::Queue submitQueue);
    void HandleTransfer(VulkanFrameContext& frameContext);

    void UpdateUniformBuffer(VulkanGraphicsFrameContext& graphicsFrameContext);
    void RecordRenderingCommands(VulkanGraphicsFrameContext& graphicsFrameContext, uint32_t swapchainImageIndex, vk::CommandBuffer commandBuffer);
    void RecordGraphicsCommands(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex); 
    template<size_t N, size_t M>
    void SubmitGraphics(VulkanGraphicsFrameContext& graphicsFrameContext, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, N>& waitSemaphores, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, M>& signalSemaphores, vk::Fence frameInFlightFence, vk::Queue submitQueue);
    void HandleGraphics(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex);
    
    void RecordTransferAndGraphicsCommands(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex);
    void HandleTransferAndGraphics(VulkanFrameContext& frameContext, uint32_t swapchainImageIndex);

    void RecreateSwapchain();

    void AdvanceFrameInFlightIndex();

    void UpdateTimingVariables();
    // Prints timing info (start of frame).
    void PrintLiveTimingInfo() const;
    // Prints frame info (end of frame).
    void PrintLiveFrameInfo(size_t currentFrameInFlightIndex) const;
    // Prints post rendering end timing info (end of rendering).
    void PrintPostCloseTimingInfo() const;
};

template<size_t N, size_t M>
void VulkanVisualizer::SubmitTransfer(VulkanTransferFrameContext& transferFrameContext, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, N>& waitSemaphores, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, M>& signalSemaphores, vk::Queue submitQueue)
{
    std::array<vk::SemaphoreSubmitInfo, N> waitSemaphoreSubmitInfos{};

    for (size_t i = 0; i < N; ++i)
    {
        waitSemaphoreSubmitInfos[i]
            .setSemaphore(waitSemaphores[i].first)
            .setStageMask(waitSemaphores[i].second);
    }


    std::array<vk::SemaphoreSubmitInfo, M> signalSemaphoreSubmitInfos{};

    for (size_t i = 0; i < M; ++i)
    {
        signalSemaphoreSubmitInfos[i]
            .setSemaphore(signalSemaphores[i].first)
            .setStageMask(signalSemaphores[i].second);
    }


    vk::CommandBufferSubmitInfo commandBufferSubmitInfo{};

    commandBufferSubmitInfo
        .setCommandBuffer(transferFrameContext.GetCommandPool().GetPrimaryCommandBuffer());


    vk::SubmitInfo2 submitInfo{};

    submitInfo
        .setWaitSemaphoreInfos(waitSemaphoreSubmitInfos)
        .setSignalSemaphoreInfos(signalSemaphoreSubmitInfos)
        .setCommandBufferInfos(commandBufferSubmitInfo);

    submitQueue.submit2(submitInfo);
}

template<size_t N, size_t M>
void VulkanVisualizer::SubmitGraphics(VulkanGraphicsFrameContext& graphicsFrameContext, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, N>& waitSemaphores, const std::array<std::pair<vk::Semaphore, vk::PipelineStageFlagBits2>, M>& signalSemaphores, vk::Fence frameInFlightFence, vk::Queue submitQueue)
{
    std::array<vk::SemaphoreSubmitInfo, N> waitSemaphoreSubmitInfos{};

    for (size_t i = 0; i < N; ++i)
    {
        waitSemaphoreSubmitInfos[i]
            .setSemaphore(waitSemaphores[i].first)
            .setStageMask(waitSemaphores[i].second);
    }


    std::array<vk::SemaphoreSubmitInfo, M> signalSemaphoreSubmitInfos{};

    for (size_t i = 0; i < M; ++i)
    {
        signalSemaphoreSubmitInfos[i]
            .setSemaphore(signalSemaphores[i].first)
            .setStageMask(signalSemaphores[i].second);
    }


    vk::CommandBufferSubmitInfo commandBufferSubmitInfo{};

    commandBufferSubmitInfo
        .setCommandBuffer(graphicsFrameContext.GetCommandPool().GetPrimaryCommandBuffer());


    vk::SubmitInfo2 submitInfo{};

    submitInfo
        .setWaitSemaphoreInfos(waitSemaphoreSubmitInfos)
        .setSignalSemaphoreInfos(signalSemaphoreSubmitInfos)
        .setCommandBufferInfos(commandBufferSubmitInfo);

    submitQueue.submit2(submitInfo, frameInFlightFence);
}

template<typename T>
T VulkanVisualizer::GetDeltaTime() const
{
    return std::chrono::duration<T>(this->deltaTime_).count();
}

template<typename T>
T VulkanVisualizer::GetSimulationTime() const
{
    return std::chrono::duration<T>(this->simulationTime_).count();
}

template<size_t SUBDIVISIONS>
void VulkanVisualizer::Draw3DSphere(float centerX, float centerY, float centerZ, float colorR, float colorG, float colorB, float colorA, float diameter)
{
    static_assert(SUBDIVISIONS <= 4, "Compile time cost is too high.");

    using Point = std::array<float, 3>;
    using TriangleIndexes = std::array<size_t, 3>;

    // Base icosahedron.

    static constexpr float phi = std::numbers::phi_v<float>;
    static constexpr float icosahedronCircumradius = ConstexprSqrt<float>(1.0f + phi*phi);

    static constexpr std::array<Point, 12> icosahedronVertices =
    {{
        {-1.0f, phi, 0.0f},
        {1.0f, phi, 0.0f},
        {-1.0f, -phi, 0.0f},
        {1.0f, -phi, 0.0f},
        {0.0f, -1.0f, phi},
        {0.0f, 1.0f, phi},
        {0.0f, -1.0f, -phi},
        {0.0f, 1.0f, -phi},
        {phi, 0.0f, -1.0f},
        {phi, 0.0f, 1.0f},
        {-phi, 0.0f, -1.0f},
        {-phi, 0.0f, 1.0f}
    }};

    static constexpr std::array<TriangleIndexes, 20> icosahedronTriangleIndexes =
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

    // Icosahedron subdivision calculation.
    static constexpr size_t triangleCount = 20zu*ConstexprPow(4zu, SUBDIVISIONS);
    // Duplicate midpoints must be accounted for (no cache is used for simplicity)!
    static constexpr size_t vertexCount = 20zu*ConstexprPow(4zu, SUBDIVISIONS) - 8zu;

    struct SubdividedSphere
    {
        std::array<glm::vec3, vertexCount> vertices{};
        std::array<TriangleIndexes, triangleCount> triangleIndexes{};
        std::array<glm::vec3, triangleCount> normals{};
    };

    // Generate the subdivided unit sphere at compile time.
    static constexpr SubdividedSphere subdividedSphere = []() constexpr
    {
        // Vectors defining sphere at a specific subdivision.
        std::vector<Point> vertices(icosahedronVertices.begin(), icosahedronVertices.end());
        std::vector<TriangleIndexes> triangleIndexes(icosahedronTriangleIndexes.begin(), icosahedronTriangleIndexes.end());
    
        // Base icosahedron vertices must be normalized as well.
        for (Point& vertex : vertices)
        {
            vertex = Normalize(vertex);
        }

        for (size_t subdivision = 0; subdivision < SUBDIVISIONS; ++subdivision)
        {
            // Indexes defining triangles will be fully replaced each subdivision (there will be four times as many).
            // On the other hand, the vertices from previous subdivisions will stay.
            std::vector<TriangleIndexes> subdividedTriangleIndexes;
            subdividedTriangleIndexes.reserve(triangleIndexes.size()*4);
    
            for (const TriangleIndexes& currentTriangleIndexes : triangleIndexes)
            {
                const size_t firstVertexIndex = currentTriangleIndexes[0];
                const size_t secondVertexIndex = currentTriangleIndexes[1];
                const size_t thirdVertexIndex = currentTriangleIndexes[2];
    
                // Each edge midpoint is projected onto the unit sphere.
                const Point firstSecondMidpoint = Normalize(Multiply(Add(vertices[firstVertexIndex], vertices[secondVertexIndex]), 0.5f));
                const Point secondThirdMidpoint = Normalize(Multiply(Add(vertices[secondVertexIndex], vertices[thirdVertexIndex]), 0.5f));
                const Point thirdFirstMidpoint = Normalize(Multiply(Add(vertices[thirdVertexIndex], vertices[firstVertexIndex]), 0.5f));
    
                // No midpoint cache is used. Adjacent faces may store duplicate vertices at identical positions.
                // Index is represented by the current biggest index + 1 (vertex is pushed in afterwards) -> size().
                const size_t firstSecondMidpointIndex = vertices.size();
                vertices.push_back(firstSecondMidpoint);
                const size_t secondThirdMidpointIndex = vertices.size();
                vertices.push_back(secondThirdMidpoint);
                const size_t thirdFirstMidpointIndex = vertices.size();
                vertices.push_back(thirdFirstMidpoint);
    
                // The original triangle is split into four triangles. The original winding is preserved.
                subdividedTriangleIndexes.push_back(TriangleIndexes{{firstVertexIndex, firstSecondMidpointIndex, thirdFirstMidpointIndex}});
                subdividedTriangleIndexes.push_back(TriangleIndexes{{secondVertexIndex, secondThirdMidpointIndex, firstSecondMidpointIndex}});
                subdividedTriangleIndexes.push_back(TriangleIndexes{{thirdVertexIndex, thirdFirstMidpointIndex, secondThirdMidpointIndex}});
                subdividedTriangleIndexes.push_back(TriangleIndexes{{firstSecondMidpointIndex, secondThirdMidpointIndex, thirdFirstMidpointIndex}});
            }
    
            // Current indexes representing triangles are the new ones.
            triangleIndexes = std::move(subdividedTriangleIndexes);
        }

        // Once all the subdivisions are done, the result is transferred
        // to the prepared arrays. Normals are calculated as well.
        SubdividedSphere finalSubdividedSphere{};

        for (size_t i = 0; i < vertexCount; ++i)
        {
            // Vertex copy.
            finalSubdividedSphere.vertices[i].x = vertices[i][0];
            finalSubdividedSphere.vertices[i].y = vertices[i][1];
            finalSubdividedSphere.vertices[i].z = vertices[i][2];
        }
    
        for (size_t i = 0; i < triangleCount; ++i)
        {
            const TriangleIndexes& currentTriangleIndexes = triangleIndexes.data()[i];

            // Triangle indexes copy.
            finalSubdividedSphere.triangleIndexes[i] = currentTriangleIndexes;

            const Point& firstVertex = vertices[currentTriangleIndexes[0]];
            const Point& secondVertex = vertices[currentTriangleIndexes[1]];
            const Point& thirdVertex = vertices[currentTriangleIndexes[2]];
    
            const Point firstSecondVector = Subtract(Point{secondVertex[0], secondVertex[1], secondVertex[2]}, Point{firstVertex[0], firstVertex[1], firstVertex[2]});
            const Point firstThirdVector = Subtract(Point{thirdVertex[0], thirdVertex[1], thirdVertex[2]}, Point{firstVertex[0], firstVertex[1], firstVertex[2]});
    
            // Normal generation.
            const Point normal = Normalize(Cross(firstSecondVector, firstThirdVector));
            finalSubdividedSphere.normals[i].x = normal[0];
            finalSubdividedSphere.normals[i].y = normal[1];
            finalSubdividedSphere.normals[i].z = normal[2];
        }

        return finalSubdividedSphere;
    }();

    // Runtime processing.

    const glm::vec3 center = {centerX, centerY, centerZ};
    const float radius = diameter*0.5f;
    // All vertices are normalized at this point.
    const float scale = radius/1.0f;
    const glm::vec4 color = {colorR, colorG, colorB, colorA};

    // Subdivided sphere's vertices are scaled and translated.
    std::array<glm::vec3, subdividedSphere.vertices.size()> vertices{};

    for (size_t i = 0; i < subdividedSphere.vertices.size(); ++i)
    {
        vertices[i] = center + subdividedSphere.vertices[i]*scale;
    }

    for (size_t i = 0; i < triangleCount; ++i)
    {
        this->DrawTriangle(vertices[subdividedSphere.triangleIndexes[i][0]], vertices[subdividedSphere.triangleIndexes[i][1]], vertices[subdividedSphere.triangleIndexes[i][2]], color, subdividedSphere.normals[i]);
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DLineSegment(float startX, float startY, float startZ, float endX, float endY, float endZ, float colorR, float colorG, float colorB, float colorA, float thickness, float capOffset, float capWidth)
{
    static_assert(SIDES >= 3, "At least three sides are required.");

    // Precalculated regular unit N-gon vertices in the XY plane.
    static constexpr std::array<glm::vec2, SIDES> nGonVertices = []()
    {
        std::array<glm::vec2, SIDES> vertices{};

        for (size_t i = 0; i < SIDES; ++i)
        {
            const float angle = 2.0f*std::numbers::pi_v<float>*(static_cast<float>(i)/static_cast<float>(SIDES));

            vertices[i] = {ConstexprCos(angle), ConstexprSin(angle)};
        }

        return vertices;
    }();

    // Precalculated outward-facing normals for the N-gon's edges in the XY plane.
    static constexpr std::array<glm::vec2, SIDES> nGonNormals = []()
    {
        std::array<glm::vec2, SIDES> normals{};

        for (size_t i = 0; i < SIDES; ++i)
        {
            const float angle = 2.0f*std::numbers::pi_v<float>*(static_cast<float>(i) + 0.5f)/static_cast<float>(SIDES);

            normals[i] = {ConstexprCos(angle), ConstexprSin(angle)};
        }

        return normals;
    }();

    const glm::vec3 startPoint = {startX, startY, startZ};
    const glm::vec3 endPoint = {endX, endY, endZ};
    const glm::vec4 color = {colorR, colorG, colorB, colorA};
    const float halfThickness = thickness/2.0f;

    const glm::vec3 startEndVector = endPoint - startPoint;
    const float length = glm::length(startEndVector);

    // Only positive length line segments with thickness are valid.
    if (length <= 2.0f*(capOffset + capWidth) || thickness <= 0.0f)
    {
        return;
    }

    const glm::vec3 normalizedStartEndVector = startEndVector/length;

    // Actual beginning and end of the line segment that account for both the capOffset and capWidth.
    const glm::vec3 adjustedStartPoint = startPoint + normalizedStartEndVector*(capOffset + capWidth);
    const glm::vec3 adjustedEndPoint = endPoint - normalizedStartEndVector*(capOffset + capWidth);

    // Choose a reference axis that is not parallel to forward.
    // Up by default, right otherwise.
    const glm::vec3 normalizedReferenceAxis = glm::abs(glm::dot(normalizedStartEndVector, this->internalConfig_.up_)) > 0.999f ? this->internalConfig_.right_ : this->internalConfig_.up_;

    // Naming based on up vector usage.
    const glm::vec3 normalizedRightVector = glm::normalize(glm::cross(normalizedReferenceAxis, normalizedStartEndVector));
    const glm::vec3 normalizedUpVector = glm::cross(normalizedStartEndVector, normalizedRightVector);

    // Final vertices.
    std::array<glm::vec3, SIDES> startCapVertices{};
    std::array<glm::vec3, SIDES> endCapVertices{};
    // Final side normals.
    std::array<glm::vec3, SIDES> sideNormals{};

    for (size_t i = 0; i < SIDES; ++i)
    {
        // Each vertex of the base N-gon is rotated into the final position using the calculated basis vectors.
        const glm::vec3 offset = normalizedRightVector*(nGonVertices[i].x*halfThickness) + normalizedUpVector*(nGonVertices[i].y*halfThickness);

        // Rotated N-gon is shifted to the start point and end point.
        startCapVertices[i] = adjustedStartPoint + offset;
        endCapVertices[i] = adjustedEndPoint + offset;

        // Each precomputed side normal is rotated into the final position using the calculated basis vectors.
        sideNormals[i] = normalizedRightVector*nGonNormals[i].x + normalizedUpVector*nGonNormals[i].y;
    }

    // Start cap and end cap.
    if (capWidth == 0.0f)
    {
        for (size_t i = 0; i < SIDES; ++i)
        {
            const size_t j = (i + 1) % SIDES;

            this->DrawTriangle(adjustedStartPoint, startCapVertices[j], startCapVertices[i], color, -normalizedStartEndVector);
            this->DrawTriangle(adjustedEndPoint, endCapVertices[i], endCapVertices[j], color, normalizedStartEndVector);
        }
    }
    else
    {
        for (size_t i = 0; i < SIDES; ++i)
        {
            const size_t j = (i + 1) % SIDES;

            // Normals are not precalculated in any way because capWidth is a runtime value.
            // Maybe there could be some sort of constexpr speed up (or speed up based on symmetries), but that is currently not implemented. 
            this->DrawTriangle(startPoint, startCapVertices[j], startCapVertices[i], color);
            this->DrawTriangle(endPoint, endCapVertices[i], endCapVertices[j], color);
        }
    }

    // Cap connections. Two triangles per face.
    for (size_t i = 0; i < SIDES; ++i)
    {
        const size_t j = (i + 1) % SIDES;
        this->DrawTriangle(startCapVertices[i], startCapVertices[j], endCapVertices[j], color, sideNormals[i]);
        this->DrawTriangle(startCapVertices[i], endCapVertices[j], endCapVertices[i], color, sideNormals[i]);
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DPolyline(std::span<const std::array<float, 3>> points, float colorR, float colorG, float colorB, float colorA, float thickness, bool isLoop)
{
    static_assert(SIDES >= 3, "At least three sides are required.");

    using Ring = std::array<glm::vec3, SIDES>;

    static constexpr float epsilon = 1e-6f;

    // Precalculated regular unit N-gon vertices in the XY plane.
    static constexpr std::array<glm::vec2, SIDES> nGonVertices = []() constexpr
    {
        std::array<glm::vec2, SIDES> vertices{};
    
        for (size_t i = 0; i < SIDES; ++i)
        {
            const float angle = 2.0f*std::numbers::pi_v<float>*(static_cast<float>(i)/static_cast<float>(SIDES));
    
            vertices[i] = {ConstexprCos(angle), ConstexprSin(angle)};
        }
    
        return vertices;
    }();

    // At least two points, positive thickness.
    if (points.size() < 2 || thickness <= 0.0f)
    {
        return;
    }

    const size_t pointCount = points.size();
    const size_t segmentCount = isLoop ? pointCount : pointCount - 1;
    const glm::vec4 color = {colorR, colorG, colorB, colorA};
    const float radius = thickness*0.5f;

    // No adjacent points can be identical.
    // This will save us from degeneracy test later.
    for (size_t i = 0; i < segmentCount; ++i)
    {
        const size_t nextIndex = (i + 1) % pointCount;
        const glm::vec3 difference = glm::vec3(points[nextIndex][0], points[nextIndex][1], points[nextIndex][2]) - glm::vec3(points[i][0], points[i][1], points[i][2]);

        if (glm::dot(difference, difference) <= epsilon*epsilon)
        {
            return;
        }
    }

    std::vector<glm::vec3> glmPoints = points | std::views::transform([](const std::array<float, 3>& point){return glm::vec3(point[0], point[1], point[2]);}) | std::ranges::to<std::vector<glm::vec3>>();

    // Ring normal at every point is calculated.
    std::vector<glm::vec3> ringNormals(pointCount);

    for (size_t i = 0; i < pointCount; ++i)
    {
        const size_t previousIndex = (i + pointCount - 1) % pointCount;
        const size_t nextIndex = (i + 1) % pointCount;

        if (isLoop == false && i == 0)
        {
            // First segment normal.
            ringNormals[i] = glmPoints[1] - glmPoints[0];
        }
        else if (isLoop == false && i == pointCount - 1)
        {
            // Last segment normal.
            ringNormals[i] = glmPoints[i] - glmPoints[i - 1];
        }
        else
        {
            // Non-end point.

            // Sum of normalized segment vectors of adjacent segments results in a bisector.
            // Because both vectors point in a same direction, the bisector does represent a normal to the ring.
            const glm::vec3 incomingVector = glm::normalize(glmPoints[i] - glmPoints[previousIndex]);
            const glm::vec3 outgoingVector = glm::normalize(glmPoints[nextIndex] - glmPoints[i]);
            const glm::vec3 bisector = incomingVector + outgoingVector;

            // A 180 degree reversal has no unique bisector.
            ringNormals[i] = glm::dot(bisector, bisector) > epsilon*epsilon ? bisector : outgoingVector;
        }

        ringNormals[i] = glm::normalize(ringNormals[i]);
    }

    // First ring is handled. It sets the ring orientation from which the other ring will follow.

    // Choose a reference axis that is not parallel to forward.
    // Up by default, right otherwise.
    const glm::vec3 normalizedReferenceAxis = glm::abs(glm::dot(ringNormals.front(), this->internalConfig_.up_)) > 0.999f ? this->internalConfig_.right_ : this->internalConfig_.up_;

    // Naming based on up vector usage.
    glm::vec3 normalizedRightVector = glm::normalize(glm::cross(normalizedReferenceAxis, ringNormals.front()));
    glm::vec3 normalizedUpVector = glm::cross(ringNormals.front(), normalizedRightVector);

    // Subsequent rings are generated.
    std::vector<Ring> rings(pointCount);

    for (size_t i = 0; i < pointCount; ++i)
    {
        glm::vec3& centerPoint = glmPoints[i];
        glm::vec3& ringNormal = ringNormals[i];
        Ring& ring = rings[i];

        // First ring's orientation is already determined.
        if (i > 0)
        {
            // Next ring's orientation is based on the previous one.

            // The new right vector is calculated as follows:
            // We want to project a normalizedRightVector onto a ring plane to see where it lands to maintain a similar orientation. 
            // The dot product is calculated between previous normalizedRightVector and a current ringNormal.
            // Because the vectors are normalized, the resulting cosine represents a projected length of normalizedRightVector onto ringNormal.
            // Then the ringNormal is set to that projected length by multiplying by the dot product.
            // This is the key part. If we now subtract this adjusted ringNormal vector from the original normalizedRightVector,
            // the entire component pointing in the normal direction is erased and the normalizedRightVector is flattened (projected)
            // into the perpendicular ring plane. That will be our new right vector.
            const glm::vec3 projectedNormalizedRightVector = normalizedRightVector - ringNormal*glm::dot(normalizedRightVector, ringNormal);

            if (glm::dot(projectedNormalizedRightVector, projectedNormalizedRightVector) > epsilon*epsilon)
            {
                normalizedRightVector = glm::normalize(projectedNormalizedRightVector);
            }
            else
            {
                // projectedNormalizedRightVector is degenerate. World axes are used as a fallback.

                // This can result in a twist because the fallback does not take previous ring into account. Even fallback must be valid.
                const glm::vec3 fallbackRightVector = std::abs(glm::dot(ringNormal, this->internalConfig_.up_)) > 0.999f ? this->internalConfig_.right_ : this->internalConfig_.up_;

                normalizedRightVector = glm::normalize(glm::cross(fallbackRightVector, ringNormal));
            }

            // Up vector is perpendicular to the previously chosen normal and right vectors.
            normalizedUpVector = glm::normalize(glm::cross(ringNormal, normalizedRightVector));
        }

        // Each ring is transformed in to a final position.
        for (size_t j = 0; j < SIDES; ++j)
        {
            const glm::vec3 offset = normalizedRightVector*(nGonVertices[j].x*radius) + normalizedUpVector*(nGonVertices[j].y*radius);

            ring[j] = centerPoint + offset;
        }
    }

    // Adjacent rings are connected with two triangles per side.
    for (size_t i = 0; i < segmentCount; ++i)
    {
        const size_t currentRingIndex = i;
        const size_t nextRingIndex = (i + 1) % pointCount;

        // We have to make sure that the connection of the first and last ring in a loop is valid.
        // The optimal solution would be to twist each ring by such an amount that the rotation
        // of the first and last ring would match. That would be computationally expensive, so a workaround
        // is used instead. A closest pair of points (one from the first ring and one from the second ring) is found.
        // The triangulation will proceed then from this pair. There could be a visible twist in the
        // triangulation, but the loop itself should be connected correctly.
        size_t offset = 0;
        if (isLoop == true && currentRingIndex == pointCount - 1)
        {
            float smallestDistance = std::numeric_limits<float>::max();

            for (size_t k = 0; k < SIDES; ++k)
            {
                for (size_t l = 0; l < SIDES; ++l)
                {
                    const glm::vec3 difference = rings[currentRingIndex][k] - rings[nextRingIndex][l];
                    const float currentDistance = glm::dot(difference, difference);

                    if (currentDistance < smallestDistance)
                    {
                        smallestDistance = currentDistance;
                        offset = (l + SIDES - k) % SIDES;
                    }
                }
            }
        }

        for (size_t j = 0; j < SIDES; ++j)
        {
            const size_t currentVertexIndex = j;
            const size_t nextVertexIndex = (j + 1) % SIDES;

            const glm::vec3& currentRingCurrentVertex = rings[currentRingIndex][currentVertexIndex];
            const glm::vec3& currentRingNextVertex = rings[currentRingIndex][nextVertexIndex];
            const glm::vec3& nextRingCurrentVertex = rings[nextRingIndex][(currentVertexIndex + offset) % SIDES];
            const glm::vec3& nextRingNextVertex = rings[nextRingIndex][(nextVertexIndex + offset) % SIDES];

            this->DrawTriangle(currentRingCurrentVertex, currentRingNextVertex, nextRingNextVertex, color);
            this->DrawTriangle(currentRingCurrentVertex, nextRingNextVertex, nextRingCurrentVertex, color);
        }
    }

    // Only open polylines have filled end caps.
    if (isLoop == false)
    {
        // Start cap.
        for (size_t i = 0; i < SIDES; ++i)
        {
            const size_t j = (i + 1) % SIDES;

            this->DrawTriangle(glmPoints.front(), rings.front()[j], rings.front()[i], color, -ringNormals.front());
        }

        // End cap.
        for (size_t i = 0; i < SIDES; ++i)
        {
            const size_t j = (i + 1) % SIDES;

            this->DrawTriangle(glmPoints.back(), rings.back()[i], rings.back()[j], color, ringNormals.back());
        }
    }
}

template<size_t SUBDIVISIONS>
void VulkanVisualizer::Draw3DPlane(float firstPointX, float firstPointY, float firstPointZ, float secondPointX, float secondPointY, float secondPointZ, float thirdPointX, float thirdPointY, float thirdPointZ, float colorR, float colorG, float colorB, float colorA)
{
    static_assert(SUBDIVISIONS > 0, "SUBDIVISIONS must be greater than zero.");

    const glm::vec3 firstPoint = {firstPointX, firstPointY, firstPointZ};
    const glm::vec3 secondPoint = {secondPointX, secondPointY, secondPointZ};
    const glm::vec3 thirdPoint = {thirdPointX, thirdPointY, thirdPointZ};
    const glm::vec4 color = {colorR, colorG, colorB, colorA};

    // Vectors defining the two adjacent edges of the parallelogram.
    const glm::vec3 firstSecondVector = secondPoint - firstPoint;
    const glm::vec3 firstThirdVector = thirdPoint - firstPoint;

    // Normal calculation.
    const glm::vec3 normal = glm::cross(firstSecondVector, firstThirdVector);
    const float normalLength = glm::length(normal);

    // Degenerate planes are rejected.
    if (normalLength == 0.0f)
    {
        return;
    }

    const glm::vec3 normalizedNormal = normal/normalLength;

    // Fourth point of the parallelogram is calculated.
    const glm::vec3 fourthPoint = secondPoint + thirdPoint - firstPoint;

    if constexpr (SUBDIVISIONS == 1)
    {
        // Front face consists of two triangles.
        this->DrawTriangle(firstPoint, secondPoint, fourthPoint, color, normalizedNormal);
        this->DrawTriangle(firstPoint, fourthPoint, thirdPoint, color, normalizedNormal);

        // Opaque planes also need a back face because of backface culling.
        if (colorA == 1.0f)
        {
            this->DrawTriangle(fourthPoint, secondPoint, firstPoint, color, -normalizedNormal);
            this->DrawTriangle(thirdPoint, fourthPoint, firstPoint, color, -normalizedNormal);
        }
    }
    else
    {
        const float step = 1.0f/static_cast<float>(SUBDIVISIONS);

        for (size_t x = 0; x < SUBDIVISIONS; ++x)
        {
            for (size_t y = 0; y < SUBDIVISIONS; ++y)
            {
                const float currentX = static_cast<float>(x)*step;
                const float currentY = static_cast<float>(y)*step;
                const float nextX = static_cast<float>(x + 1)*step;
                const float nextY = static_cast<float>(y + 1)*step;

                const glm::vec3 p0 = firstPoint + firstSecondVector*currentX + firstThirdVector*currentY;
                const glm::vec3 p1 = firstPoint + firstSecondVector*nextX + firstThirdVector*currentY;
                const glm::vec3 p2 = firstPoint + firstSecondVector*nextX + firstThirdVector*nextY;
                const glm::vec3 p3 = firstPoint + firstSecondVector*currentX + firstThirdVector*nextY;

                // Front face consists of two triangles.
                this->DrawTriangle(p0, p1, p2, color, normalizedNormal);
                this->DrawTriangle(p0, p2, p3, color, normalizedNormal);

                // Opaque planes also need a back face because of backface culling.
                if (colorA == 1.0f)
                {
                    this->DrawTriangle(p2, p1, p0, color, -normalizedNormal);
                    this->DrawTriangle(p3, p2, p0, color, -normalizedNormal);
                }
            }
        }
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DDisk(float centerX, float centerY, float centerZ, float directionX, float directionY, float directionZ, float colorR, float colorG, float colorB, float colorA, float diameter)
{
    static_assert(SIDES >= 3, "At least three sides are required.");

    // Precalculated regular unit N-gon vertices in the XY plane.
    static constexpr std::array<glm::vec2, SIDES> nGonVertices = []()
    {
        std::array<glm::vec2, SIDES> vertices{};

        for (size_t i = 0; i < SIDES; ++i)
        {
            const float angle = 2.0f*std::numbers::pi_v<float>*(static_cast<float>(i)/static_cast<float>(SIDES));

            vertices[i] = {ConstexprCos(angle), ConstexprSin(angle)};
        }

        return vertices;
    }();

    const glm::vec3 centerPoint = {centerX, centerY, centerZ};
    const glm::vec3 directionPoint = {directionX, directionY, directionZ};
    const glm::vec4 color = {colorR, colorG, colorB, colorA};
    const float radius = diameter/2.0f;

    const glm::vec3 directionVector = directionPoint - centerPoint;
    const float length = glm::length(directionVector);

    // A nonzero direction and positive width are required.
    if (length == 0.0f || radius <= 0.0f)
    {
        return;
    }

    const glm::vec3 normalizedDirectionVector = directionVector/length;

    // Choose a reference axis that is not parallel to the direction.
    // Up by default, right otherwise.
    const glm::vec3 normalizedReferenceAxis = glm::abs(glm::dot(normalizedDirectionVector, this->internalConfig_.up_)) > 0.999f ? this->internalConfig_.right_ : this->internalConfig_.up_;

    // Naming based on up vector usage.
    const glm::vec3 normalizedRightVector = glm::normalize(glm::cross(normalizedReferenceAxis, normalizedDirectionVector));
    const glm::vec3 normalizedUpVector = glm::cross(normalizedDirectionVector, normalizedRightVector);

    // Final disk vertices.
    std::array<glm::vec3, SIDES> diskVertices{};

    for (size_t i = 0; i < SIDES; ++i)
    {
        const glm::vec3 offset = normalizedRightVector*(nGonVertices[i].x*radius) + normalizedUpVector*(nGonVertices[i].y*radius);

        diskVertices[i] = centerPoint + offset;
    }

    // Front face triangulation.
    for (size_t i = 0; i < SIDES; ++i)
    {
        const size_t j = (i + 1) % SIDES;

        this->DrawTriangle(centerPoint, diskVertices[j], diskVertices[i], color, -normalizedDirectionVector);
    }

    // Opaque disks also need a back face because of backface culling.
    if (colorA == 1.0f)
    {
        for (size_t i = 0; i < SIDES; ++i)
        {
            const size_t j = (i + 1) % SIDES;

            this->DrawTriangle(centerPoint, diskVertices[i], diskVertices[j], color, normalizedDirectionVector);
        }
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DCone(float baseCenterX, float baseCenterY, float baseCenterZ, float tipX, float tipY, float tipZ, float colorR, float colorG, float colorB, float colorA, float baseWidth)
{
    static_assert(SIDES >= 3, "At least three sides are required.");

    // Precalculated regular unit N-gon vertices in the XY plane.
    static constexpr std::array<glm::vec2, SIDES> nGonVertices = []()
    {
        std::array<glm::vec2, SIDES> vertices{};

        for (size_t i = 0; i < SIDES; ++i)
        {
            const float angle = 2.0f*std::numbers::pi_v<float>*(static_cast<float>(i)/static_cast<float>(SIDES));

            vertices[i] = {ConstexprCos(angle), ConstexprSin(angle)};
        }

        return vertices;
    }();

    const glm::vec3 baseCenterPoint = {baseCenterX, baseCenterY, baseCenterZ};
    const glm::vec3 tipPoint = {tipX, tipY, tipZ};
    const glm::vec4 color = {colorR, colorG, colorB, colorA};
    const float halfBaseWidth = baseWidth/2.0f;

    const glm::vec3 baseCenterTipVector = tipPoint - baseCenterPoint;
    const float length = glm::length(baseCenterTipVector);

    // Only positive length cones and positive base widths are valid.
    if (length == 0.0f || halfBaseWidth <= 0.0f)
    {
        return;
    }

    const glm::vec3 normalizedBaseCenterTipVector = baseCenterTipVector/length;

    // Choose a reference axis that is not parallel to forward.
    // Up by default, right otherwise.
    const glm::vec3 normalizedReferenceAxis = glm::abs(glm::dot(normalizedBaseCenterTipVector, this->internalConfig_.up_)) > 0.999f ? this->internalConfig_.right_ : this->internalConfig_.up_;

    // Naming based on up vector usage.
    const glm::vec3 normalizedRightVector = glm::normalize(glm::cross(normalizedReferenceAxis, normalizedBaseCenterTipVector));
    const glm::vec3 normalizedUpVector = glm::cross(normalizedBaseCenterTipVector, normalizedRightVector);

    // Final base vertices.
    std::array<glm::vec3, SIDES> baseVertices{};

    for (size_t i = 0; i < SIDES; ++i)
    {
        // Each vertex of the base N-gon is rotated into a final position using the calculated basis vectors.
        const glm::vec3 offset = normalizedRightVector*(nGonVertices[i].x*halfBaseWidth) + normalizedUpVector*(nGonVertices[i].y*halfBaseWidth);

        // Rotated N-gon is shifted to the base center.
        baseVertices[i] = baseCenterPoint + offset;
    }

    // Base cap.
    for (size_t i = 0; i < SIDES; ++i)
    {
        const size_t j = (i + 1) % SIDES;

        this->DrawTriangle(baseCenterPoint, baseVertices[j], baseVertices[i], color, -normalizedBaseCenterTipVector);
    }

    // Cone sides. Each triangle connects two adjacent base vertices to the apex.
    for (size_t i = 0; i < SIDES; ++i)
    {
        const size_t j = (i + 1) % SIDES;

        this->DrawTriangle(baseVertices[i], baseVertices[j], tipPoint, color);
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DArrow(float startX, float startY, float startZ, float endX, float endY, float endZ, float colorR, float colorG, float colorB, float colorA, float thickness)
{
    static_assert(SIDES >= 3, "At least three sides are required.");

    const glm::vec3 startPoint = {startX, startY, startZ};
    const glm::vec3 endPoint = {endX, endY, endZ};

    const glm::vec3 directionVector = endPoint - startPoint;
    const float arrowLength = glm::length(directionVector);

    if (arrowLength <= 0.0f || thickness <= 0.0f)
    {
        return;
    }

    const glm::vec3 normalizedDirectionVector = directionVector/arrowLength;

    const float radius = thickness*0.5f;
    const float tipRadius = radius*2.0f;

    // The length of the tip of the arrow is proportionally bounded by radius and total length.
    // If the arrow is too long, it is bounded by the radius.
    // If the arrow is too short, it is bounded by the arrow length.
    const float tipLength = std::min(radius*8.0f, arrowLength*0.4f);

    const glm::vec3 tipBasePoint = endPoint - normalizedDirectionVector*tipLength;

    // The body is drawn.
    this->Draw3DLineSegment<SIDES>(startPoint.x, startPoint.y, startPoint.z, tipBasePoint.x, tipBasePoint.y, tipBasePoint.z, colorR, colorG, colorB, colorA, thickness);

    // The arrowhead is drawn.
    this->Draw3DCone<SIDES>(tipBasePoint.x, tipBasePoint.y, tipBasePoint.z, endPoint.x, endPoint.y, endPoint.z, colorR, colorG, colorB, colorA, tipRadius*2.0f);
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DAxes(float originX, float originY, float originZ, float colorA, float length, float thickness)
{
    const float halfThickness = thickness/2.0f;

    this->Draw3DCone<SIDES>(originX + halfThickness, originY, originZ, originX, originY, originZ, 1.0f, 0.0f, 0.0f, colorA, thickness);
    this->Draw3DArrow<SIDES>(originX + halfThickness, originY, originZ, originX + this->internalConfig_.right_.x*length + halfThickness, originY, originZ, 1.0f, 0.0f, 0.0f, colorA, thickness);

    this->Draw3DCone<SIDES>(originX, originY + halfThickness, originZ, originX, originY, originZ, 0.0f, 0.0f, 1.0f, colorA, thickness);
    this->Draw3DArrow<SIDES>(originX, originY + halfThickness, originZ, originX, originY + this->internalConfig_.forward_.y*length + halfThickness, originZ, 0.0f, 0.0f, 1.0f, colorA, thickness);

    this->Draw3DCone<SIDES>(originX, originY, originZ + halfThickness, originX, originY, originZ, 0.0f, 1.0f, 0.0f, colorA, thickness);
    this->Draw3DArrow<SIDES>(originX, originY, originZ + halfThickness, originX, originY, originZ + this->internalConfig_.up_.z*length + halfThickness, 0.0f, 1.0f, 0.0f, colorA, thickness);
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DWireframe(float originX, float originY, float originZ, float rightX, float rightY, float rightZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ, float colorR, float colorG, float colorB, float colorA, float thickness)
{
    const glm::vec3 originPoint = {originX, originY, originZ};
    const glm::vec3 rightPoint = {rightX, rightY, rightZ};
    const glm::vec3 forwardPoint = {forwardX, forwardY, forwardZ};
    const glm::vec3 upPoint = {upX, upY, upZ};

    const std::array<glm::vec3, 8> vertices =
    {
        originPoint,
        rightPoint,
        forwardPoint,
        upPoint,
        rightPoint + forwardPoint - originPoint,
        rightPoint + upPoint - originPoint,
        forwardPoint + upPoint - originPoint,
        rightPoint + forwardPoint + upPoint - 2.0f*originPoint
    };

    static constexpr std::array<std::array<size_t, 2>, 12> edgeIndexes =
    {{
        {0, 1},
        {2, 4},
        {3, 5},
        {6, 7},
        {0, 2},
        {1, 4},
        {3, 6},
        {5, 7},
        {0, 3},
        {1, 5},
        {2, 6},
        {4, 7}
    }};

    for (const std::array<size_t, 2>&edgeIndex : edgeIndexes)
    {
        const glm::vec3& startPoint = vertices[edgeIndex[0]];
        const glm::vec3& endPoint = vertices[edgeIndex[1]];

        this->Draw3DLineSegment<SIDES>(startPoint.x, startPoint.y, startPoint.z, endPoint.x, endPoint.y, endPoint.z, colorR, colorG, colorB, colorA, thickness, (thickness/2.0f)*1.05f);
    }

    for (const glm::vec3& vertex : vertices)
    {
        this->Draw3DIcosahedron(vertex.x, vertex.y, vertex.z, colorR, colorG, colorB, colorA, thickness);
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw3DGrid(float originX, float originY, float originZ, float rightX, float rightY, float rightZ, float forwardX, float forwardY, float forwardZ, float colorR, float colorG, float colorB, float colorA, float thickness, size_t n, size_t m)
{
    if (n == 0 || m == 0)
    {
        return;
    }

    const glm::vec3 originPoint = {originX, originY, originZ};
    const glm::vec3 rightPoint = {rightX, rightY, rightZ};
    const glm::vec3 forwardPoint = {forwardX, forwardY, forwardZ};

    const glm::vec3 rightVector = rightPoint - originPoint;
    const glm::vec3 forwardVector = forwardPoint - originPoint;

    auto getGridPoint = [&](size_t x, size_t y) -> glm::vec3
    {
        return originPoint + rightVector*static_cast<float>(x) + forwardVector*static_cast<float>(y);
    };

    for (size_t y = 0; y <= m; ++y)
    {
        for (size_t x = 0; x <= n; ++x)
        {
            const glm::vec3 currentPoint = getGridPoint(x, y);

            this->Draw3DIcosahedron(currentPoint.x, currentPoint.y, currentPoint.z, colorR, colorG, colorB, colorA, thickness);

            if (x < n)
            {
                const glm::vec3 rightNeighborPoint = getGridPoint(x + 1, y);

                this->Draw3DLineSegment<SIDES>(currentPoint.x, currentPoint.y, currentPoint.z, rightNeighborPoint.x, rightNeighborPoint.y, rightNeighborPoint.z, colorR, colorG, colorB, colorA, thickness, (thickness/2.0f)*1.05f);
            }

            if (y < m)
            {
                const glm::vec3 forwardNeighborPoint = getGridPoint(x, y + 1);

                this->Draw3DLineSegment<SIDES>(currentPoint.x, currentPoint.y, currentPoint.z, forwardNeighborPoint.x, forwardNeighborPoint.y, forwardNeighborPoint.z, colorR, colorG, colorB, colorA, thickness, (thickness/2.0f)*1.05f);
            }
        }
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw2DPoint(float centerX, float centerY, float Z, float colorR, float colorG, float colorB, float colorA, float diameter)
{
    static_assert(SIDES >= 3, "At least three sides are required.");

    // Precalculated regular unit N-gon vertices in the XY plane.
    static constexpr std::array<glm::vec2, SIDES> nGonVertices = []()
    {
        std::array<glm::vec2, SIDES> vertices{};
    
        for (size_t i = 0; i < SIDES; ++i)
        {
            const float angle = 2.0f * std::numbers::pi_v<float>*(static_cast<float>(i)/static_cast<float>(SIDES));
    
            vertices[i] = {ConstexprCos(angle), ConstexprSin(angle)};
        }
    
        return vertices;
    }();

    const glm::vec3 centerPoint = {centerX, centerY, Z};
    const glm::vec4 color = {colorR, colorG, colorB, colorA};
    const float radius = diameter*0.5f;

    if (radius <= 0.0f)
    {
        return;
    }

    // Final point vertices.
    std::array<glm::vec3, SIDES> pointVertices{};

    for (size_t i = 0; i < SIDES; ++i)
    {
        pointVertices[i] = centerPoint + glm::vec3(nGonVertices[i].x*radius, nGonVertices[i].y*radius, 0.0f);
    }

    // N-gon triangulation.
    for (size_t i = 1; i < SIDES - 1; ++i)
    {
        this->DrawTriangle(pointVertices[0], pointVertices[i], pointVertices[i + 1], color, this->internalConfig_.up_);
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw2DWireframe(float originX, float originY, float rightX, float rightY, float forwardX, float forwardY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness)
{
    const glm::vec2 originPoint = {originX, originY};
    const glm::vec2 rightPoint = {rightX, rightY};
    const glm::vec2 forwardPoint = {forwardX, forwardY};

    const glm::vec2 oppositeCornerPoint = rightPoint + forwardPoint - originPoint;

    const std::array<glm::vec2, 4> vertices =
    {
        originPoint,
        rightPoint,
        forwardPoint,
        oppositeCornerPoint
    };

    static constexpr std::array<std::array<size_t, 2>, 4> edgeIndexes =
    {{
        {0, 1},
        {2, 3},
        {0, 2},
        {1, 3}
    }};

    for (const std::array<size_t, 2>&edgeIndex : edgeIndexes)
    {
        const glm::vec2& startPoint = vertices[edgeIndex[0]];
        const glm::vec2& endPoint = vertices[edgeIndex[1]];

        this->Draw2DLineSegment(startPoint.x, startPoint.y, endPoint.x, endPoint.y, Z, colorR, colorG, colorB, colorA, thickness, (thickness/2.0f)*1.05f);
    }

    for (const glm::vec2& vertex : vertices)
    {
        this->Draw2DPoint<SIDES>(vertex.x, vertex.y, Z, colorR, colorG, colorB, colorA, thickness);
    }
}

template<size_t SIDES>
void VulkanVisualizer::Draw2DGrid(float originX, float originY, float rightX, float rightY, float forwardX, float forwardY, float Z, float colorR, float colorG, float colorB, float colorA, float thickness, size_t n, size_t m)
{
    if (n == 0 || m == 0)
    {
        return;
    }

    const glm::vec2 originPoint = {originX, originY};
    const glm::vec2 rightPoint = {rightX, rightY};
    const glm::vec2 forwardPoint = {forwardX, forwardY};

    const glm::vec2 rightVector = rightPoint - originPoint;
    const glm::vec2 forwardVector = forwardPoint - originPoint;

    auto getGridPoint = [&](size_t x, size_t y) -> glm::vec2
    {
        return originPoint + rightVector*static_cast<float>(x) + forwardVector*static_cast<float>(y);
    };

    for (size_t y = 0; y <= m; ++y)
    {
        for (size_t x = 0; x <= n; ++x)
        {
            const glm::vec2 currentPoint = getGridPoint(x, y);

            this->Draw2DPoint<SIDES>(currentPoint.x, currentPoint.y, Z, colorR, colorG, colorB, colorA, thickness);

            if (x < n)
            {
                const glm::vec2 rightNeighborPoint = getGridPoint(x + 1, y);

                this->Draw2DLineSegment(currentPoint.x, currentPoint.y, rightNeighborPoint.x, rightNeighborPoint.y, Z, colorR, colorG, colorB, colorA, thickness, (thickness/2.0f)*1.05f);
            }

            if (y < m)
            {
                const glm::vec2 forwardNeighborPoint = getGridPoint(x, y + 1);

                this->Draw2DLineSegment(currentPoint.x, currentPoint.y, forwardNeighborPoint.x, forwardNeighborPoint.y, Z, colorR, colorG, colorB, colorA, thickness, (thickness/2.0f)*1.05f);
            }
        }
    }
}
