
#pragma once

#include <vulkan/vulkan.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

#include "VulkanInternalConfig.h"
#include "VulkanHelperEnums.h"


class VulkanLoggerConfig
{
public:

	bool showMetaInformation_ = false;

public:

	VulkanLoggerConfig();
	~VulkanLoggerConfig();

	VulkanLoggerConfig(const VulkanLoggerConfig& loggerConfig) = delete;
	VulkanLoggerConfig(VulkanLoggerConfig&& loggerConfig) noexcept = delete;
	VulkanLoggerConfig& operator=(const VulkanLoggerConfig& loggerConfig) = delete;
	VulkanLoggerConfig& operator=(VulkanLoggerConfig&& loggerConfig) noexcept = delete;

	void SetShowMetaInformation(bool showMetaInformation);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanWindowConfig
{
public:

	std::string windowName_ = "Unnamed";
	VulkanWindowMode windowMode_ = VulkanWindowMode::Window;
	int windowWidth_ = 1280;
	int windowHeight_ = 720;
	int windowX_ = -1;
	int windowY_ = -1;

public:

	VulkanWindowConfig();
	~VulkanWindowConfig();

	VulkanWindowConfig(const VulkanWindowConfig& windowConfig) = delete;
	VulkanWindowConfig(VulkanWindowConfig&& windowConfig) noexcept = delete;
	VulkanWindowConfig& operator=(const VulkanWindowConfig& windowConfig) = delete;
	VulkanWindowConfig& operator=(VulkanWindowConfig&& windowConfig) noexcept = delete;

	void SetWindowName(std::string windowName);
	void SetWindowMode(VulkanWindowMode windowMode);
	void SetWindowWidth(int windowWidth);
	void SetWindowHeight(int windowHeight);
	void SetWindowX(int windowX);
	void SetWindowY(int windowY);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanInstanceConfig
{
public:

	std::string applicationName_ = "Unnamed";
	uint32_t minimumVulkanVersion_ = vk::makeVersion(1, 0, 0);
	bool showMessengerInfoMessages_ = false;
	bool showMessengerVerboseMessages_ = false;
	bool showAvailableLayersAndExtensions_ = false;
	bool enableValidationLayers_ = false;
	bool useLayerSettings_ = false;

public:

	VulkanInstanceConfig();
	~VulkanInstanceConfig();

	VulkanInstanceConfig(const VulkanInstanceConfig& instanceConfig) = delete;
	VulkanInstanceConfig(VulkanInstanceConfig&& instanceConfig) noexcept = delete;
	VulkanInstanceConfig& operator=(const VulkanInstanceConfig& instanceConfig) = delete;
	VulkanInstanceConfig& operator=(VulkanInstanceConfig&& instanceConfig) noexcept = delete;

	void SetApplicationName(std::string applicationName);
	void SetMinimumVulkanVersion(uint32_t minimumVulkanVersion);
	void SetShowMessengerInfoMessages(bool showMessengerInfoMessages);
	void SetShowMessengerVerboseMessages(bool showMessengerVerboseMessages);
	void SetShowAvailableLayersAndExtensions(bool showAvailableLayersAndExtensions);
	void SetEnableValidationLayers(bool enableValidationLayers);
	void SetUseLayerSettings(bool useLayerSettings);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanDeviceConfig
{
public:

	bool showAvailablePhysicalDevices_ = false;
	uint32_t minimumVulkanVersion_ = vk::makeVersion(1, 0, 0);
	vk::SampleCountFlagBits requiredMSAASampleCount_ = vk::SampleCountFlagBits::e1;
	bool sampleRateShadingEnabled_ = false;

public:

	VulkanDeviceConfig();
	~VulkanDeviceConfig();

	VulkanDeviceConfig(const VulkanDeviceConfig& deviceConfig) = delete;
	VulkanDeviceConfig(VulkanDeviceConfig&& deviceConfig) noexcept = delete;
	VulkanDeviceConfig& operator=(const VulkanDeviceConfig& deviceConfig) = delete;
	VulkanDeviceConfig& operator=(VulkanDeviceConfig&& deviceConfig) noexcept = delete;

	void SetMinimumVulkanVersion(uint32_t minimumVulkanVersion);
	void SetShowAvailablePhysicalDevices(bool showAvailablePhysicalDevices);

	void SetMSAARequirements(vk::SampleCountFlagBits requiredMSAASampleCount, bool sampleRateShadingEnabled);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanSwapchainConfig
{
public:

public:

	VulkanSwapchainConfig();
	~VulkanSwapchainConfig();

	VulkanSwapchainConfig(const VulkanSwapchainConfig& swapchainConfig) = delete;
	VulkanSwapchainConfig(VulkanSwapchainConfig&& swapchainConfig) noexcept = delete;
	VulkanSwapchainConfig& operator=(const VulkanSwapchainConfig& swapchainConfig) = delete;
	VulkanSwapchainConfig& operator=(VulkanSwapchainConfig&& swapchainConfig) noexcept = delete;
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanShaderModuleConfig
{
public:

	bool compileShaderAtRuntime_ = false;
	bool compileShaderWithDebugFlags_ = false;
	uint32_t minimumVulkanVersion_ = vk::makeVersion(1, 0, 0);

public:

	VulkanShaderModuleConfig();
	~VulkanShaderModuleConfig();

	VulkanShaderModuleConfig(const VulkanShaderModuleConfig& shaderModuleConfig) = delete;
	VulkanShaderModuleConfig(VulkanShaderModuleConfig&& shaderModuleConfig) noexcept = delete;
	VulkanShaderModuleConfig& operator=(const VulkanShaderModuleConfig& shaderModuleConfig) = delete;
	VulkanShaderModuleConfig& operator=(VulkanShaderModuleConfig&& shaderModuleConfig) noexcept = delete;

	void SetCompileShaderAtRuntime(bool compileShaderAtRuntime);
	void SetCompileShaderWithDebugFlags(bool compileShaderWithDebugFlags);
	void SetMinimumVulkanVersion(uint32_t minimumVulkanVersion);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanPipelineConfig
{
public:

	std::unordered_map<VulkanPipelineUseCase, vk::SampleCountFlagBits> rasterizationSamples_;
	std::unordered_map<VulkanPipelineUseCase, bool> sampleShadingEnable_;
	std::unordered_map<VulkanPipelineUseCase, float> minSampleShading_;

public:

	VulkanPipelineConfig();
	~VulkanPipelineConfig();

	VulkanPipelineConfig(const VulkanPipelineConfig& pipelineConfig) = delete;
	VulkanPipelineConfig(VulkanPipelineConfig&& pipelineConfig) noexcept = delete;
	VulkanPipelineConfig& operator=(const VulkanPipelineConfig& pipelineConfig) = delete;
	VulkanPipelineConfig& operator=(VulkanPipelineConfig&& pipelineConfig) noexcept = delete;

	void SetMSAAProperties(vk::SampleCountFlagBits rasterizationSamples, bool sampleShadingEnable, float minSampleShading);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanBufferConfig
{
public:

	std::unordered_map<VulkanBufferUseCase, vk::DeviceSize> bufferSizes_;

public:

	VulkanBufferConfig();
	~VulkanBufferConfig();

	VulkanBufferConfig(const VulkanBufferConfig& bufferConfig) = delete;
	VulkanBufferConfig(VulkanBufferConfig&& bufferConfig) noexcept = delete;
	VulkanBufferConfig& operator=(const VulkanBufferConfig& bufferConfig) = delete;
	VulkanBufferConfig& operator=(VulkanBufferConfig&& bufferConfig) noexcept = delete;

	void SetBufferSize(VulkanBufferUseCase bufferUseCase, vk::DeviceSize bufferSize);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanImageConfig
{
public:

	std::unordered_map<VulkanImageUseCase, vk::SampleCountFlagBits> imageSampleCounts_;

public:

	VulkanImageConfig();
	~VulkanImageConfig();

	VulkanImageConfig(const VulkanImageConfig& imageConfig) = delete;
	VulkanImageConfig(VulkanImageConfig&& imageConfig) noexcept = delete;
	VulkanImageConfig& operator=(const VulkanImageConfig& imageConfig) = delete;
	VulkanImageConfig& operator=(VulkanImageConfig&& imageConfig) noexcept = delete;

	void SetImageSampleCount(VulkanImageUseCase imageUseCase, vk::SampleCountFlagBits imageSampleCount);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanCameraConfig
{
public:

	glm::vec3 position_ = {0.0f, -5.0f, 0.0f};
	glm::vec3 targetPosition_ = {0.0f, 0.0f, 0.0f};
	glm::vec3 up_ = {0.0f, 0.0f, 1.0f};
	glm::vec3 upFallback_ = {0.0f, 1.0f, 0.0f};

	float fieldOfView_ = glm::radians(60.0f);
	float nearPlane_ = 0.1f;
	float farPlane_ = 200.0f;

public:

	VulkanCameraConfig();
	~VulkanCameraConfig();

	VulkanCameraConfig(const VulkanCameraConfig& cameraConfig) = delete;
	VulkanCameraConfig(VulkanCameraConfig&& cameraConfig) noexcept = delete;
	VulkanCameraConfig& operator=(const VulkanCameraConfig& cameraConfig) = delete;
	VulkanCameraConfig& operator=(VulkanCameraConfig&& cameraConfig) noexcept = delete;

	void SetViewMatrixComponents(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 up, glm::vec3 upFallback);
	void SetPerspectiveMatrixComponents(float fieldOfView, float nearPlane, float farPlane);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanInputHandlerConfig
{
public:

	bool cursorIsCaptured_ = false;

public:

	VulkanInputHandlerConfig();
	~VulkanInputHandlerConfig();

	VulkanInputHandlerConfig(const VulkanInputHandlerConfig& inputHandlerConfig) = delete;
	VulkanInputHandlerConfig(VulkanInputHandlerConfig&& inputHandlerConfig) noexcept = delete;
	VulkanInputHandlerConfig& operator=(const VulkanInputHandlerConfig& inputHandlerConfig) = delete;
	VulkanInputHandlerConfig& operator=(VulkanInputHandlerConfig&& inputHandlerConfig) noexcept = delete;

	void SetCursorIsCaptured(bool cursorIsCaptured);
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanVisualizerConfig
{
public:

	VulkanInternalConfig internalConfig_{};

	VulkanLoggerConfig loggerConfig_;
	VulkanWindowConfig windowConfig_;
	VulkanInstanceConfig instanceConfig_;
	VulkanDeviceConfig deviceConfig_;
	VulkanSwapchainConfig swapchainConfig_;
	VulkanShaderModuleConfig shaderModuleConfig_;
	VulkanPipelineConfig pipelineConfig_;
	VulkanBufferConfig bufferConfig_;
	VulkanImageConfig imageConfig_;
	VulkanCameraConfig cameraConfig_;
	VulkanInputHandlerConfig inputHandlerConfig_;

public:

	VulkanVisualizerConfig();
	~VulkanVisualizerConfig();

	VulkanVisualizerConfig(const VulkanVisualizerConfig& visualizerConfig) = delete;
	VulkanVisualizerConfig(VulkanVisualizerConfig&& visualizerConfig) noexcept = delete;
	VulkanVisualizerConfig& operator=(const VulkanVisualizerConfig& visualizerConfig) = delete;
	VulkanVisualizerConfig& operator=(VulkanVisualizerConfig&& visualizerConfig) noexcept = delete;

	void SetLightSourceComponents(glm::vec3 lightSourcePosition, glm::vec3 lightSourceColor);
	void SetPhongParameters(bool usePhongReflectionModel, float phongAmbient, float phongShininess);

	void SetMaxVertexCount(size_t maxVertexCount);

	void SetFramesInFlightCount(size_t framesInFlightCount);

	void UseIndexedDraw(bool useIndexedDraw);

	void UseMSAA(bool useMSAA, vk::SampleCountFlagBits msaaSampleCount = vk::SampleCountFlagBits::e1, bool enableSampleShading = false, float minSampleShading = 0.0f);

	void SetShowLiveTimingInfo(bool showLiveTimingInfo);
	void SetShowLiveFrameInfo(bool showLiveFrameInfo);
	void SetShowPostCloseTimingInfo(bool showPostCloseTimingInfo);

	void SetVisualizerMode(bool is2D);

	void SetShowMetaInformation_Logger(bool showMetaInformation);

	void SetRendererName(std::string name);
	void SetMinimumVulkanVersion(uint32_t minimumVulkanVersion);

	void SetWindowProperties_Window(VulkanWindowMode windowMode, int windowWidth, int windowHeight, int windowX, int windowY);

	void SetShowMessengerInfoMessages_Instance(bool showMessengerInfoMessages);
	void SetShowMessengerVerboseMessages_Instance(bool showMessengerVerboseMessages);
	void SetShowAvailableLayersAndExtensions_Instance(bool showAvailableLayersAndExtensions);
	void SetEnableValidationLayers_Instance(bool enableValidationLayers);
	void SetUseLayerSettings_Instance(bool useLayerSettings);

	void SetShowAvailablePhysicalDevices_Device(bool showAvailablePhysicalDevices);

	void SetCompileShaderAtRuntime_ShaderModule(bool compileShaderAtRuntime);
	void SetCompileShaderWithDebugFlags_ShaderModule(bool compileShaderWithDebugFlags);

	void SetBufferSize_Buffer(VulkanBufferUseCase bufferUseCase, vk::DeviceSize bufferSize);

	void SetViewMatrixComponents_Camera(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 up, glm::vec3 upFallback);
	void SetPerspectiveMatrixComponents_Camera(float fieldOfView, float nearPlane, float farPlane);

	void SetCursorIsCaptured_InputHandler(bool cursorIsCaptured);
};
