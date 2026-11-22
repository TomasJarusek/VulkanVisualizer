
#include "VulkanConfig.h"

#include "VulkanVertex.h"


VulkanLoggerConfig::VulkanLoggerConfig()
{
}

VulkanLoggerConfig::~VulkanLoggerConfig()
{
}

void VulkanLoggerConfig::SetShowMetaInformation(bool showMetaInformation)
{
	this->showMetaInformation_ = showMetaInformation;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanWindowConfig::VulkanWindowConfig()
{
}

VulkanWindowConfig::~VulkanWindowConfig()
{
}

void VulkanWindowConfig::SetWindowName(std::string windowName)
{
	this->windowName_ = windowName;
}

void VulkanWindowConfig::SetWindowMode(VulkanWindowMode windowMode)
{
	this->windowMode_ = windowMode;
}

void VulkanWindowConfig::SetWindowWidth(int windowWidth)
{
	this->windowWidth_ = windowWidth;
}

void VulkanWindowConfig::SetWindowHeight(int windowHeight)
{
	this->windowHeight_ = windowHeight;
}

void VulkanWindowConfig::SetWindowX(int windowX)
{
	this->windowX_ = windowX;
}

void VulkanWindowConfig::SetWindowY(int windowY)
{
	this->windowY_ = windowY;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanInstanceConfig::VulkanInstanceConfig()
{
}

VulkanInstanceConfig::~VulkanInstanceConfig()
{
}

void VulkanInstanceConfig::SetApplicationName(std::string applicationName)
{
	this->applicationName_ = applicationName;
}

void VulkanInstanceConfig::SetMinimumVulkanVersion(uint32_t minimumVulkanVersion)
{
	this->minimumVulkanVersion_ = minimumVulkanVersion;
}

void VulkanInstanceConfig::SetShowMessengerInfoMessages(bool showMessengerInfoMessages)
{
	this->showMessengerInfoMessages_ = showMessengerInfoMessages;
}

void VulkanInstanceConfig::SetShowMessengerVerboseMessages(bool showMessengerVerboseMessages)
{
	this->showMessengerVerboseMessages_ = showMessengerVerboseMessages;
}
void VulkanInstanceConfig::SetShowAvailableLayersAndExtensions(bool showAvailableLayersAndExtensions)
{
	this->showAvailableLayersAndExtensions_ = showAvailableLayersAndExtensions;
}

void VulkanInstanceConfig::SetEnableValidationLayers(bool enableValidationLayers)
{
	this->enableValidationLayers_ = enableValidationLayers;
}

void VulkanInstanceConfig::SetUseLayerSettings(bool useLayerSettings)
{
	this->useLayerSettings_ = useLayerSettings;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanDeviceConfig::VulkanDeviceConfig()
{
}

VulkanDeviceConfig::~VulkanDeviceConfig()
{
}

void VulkanDeviceConfig::SetMinimumVulkanVersion(uint32_t minimumVulkanVersion)
{
	this->minimumVulkanVersion_ = minimumVulkanVersion;
}

void VulkanDeviceConfig::SetShowAvailablePhysicalDevices(bool showAvailablePhysicalDevices)
{
	this->showAvailablePhysicalDevices_ = showAvailablePhysicalDevices;
}

void VulkanDeviceConfig::SetMSAARequirements(vk::SampleCountFlagBits requiredMSAASampleCount, bool sampleRateShadingEnabled)
{
	this->requiredMSAASampleCount_ = requiredMSAASampleCount;
	this->sampleRateShadingEnabled_ = sampleRateShadingEnabled;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanSwapchainConfig::VulkanSwapchainConfig()
{
}

VulkanSwapchainConfig::~VulkanSwapchainConfig()
{
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanShaderModuleConfig::VulkanShaderModuleConfig()
{
}

VulkanShaderModuleConfig::~VulkanShaderModuleConfig()
{
}

void VulkanShaderModuleConfig::SetCompileShaderAtRuntime(bool compileShaderAtRuntime)
{
	this->compileShaderAtRuntime_ = compileShaderAtRuntime;
}

void VulkanShaderModuleConfig::SetCompileShaderWithDebugFlags(bool compileShaderWithDebugFlags)
{
	this->compileShaderWithDebugFlags_ = compileShaderWithDebugFlags;
}

void VulkanShaderModuleConfig::SetMinimumVulkanVersion(uint32_t minimumVulkanVersion)
{
	this->minimumVulkanVersion_ = minimumVulkanVersion;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanPipelineConfig::VulkanPipelineConfig()
{
	this->rasterizationSamples_[VulkanPipelineUseCase::Opaque] = vk::SampleCountFlagBits::e1;
	this->sampleShadingEnable_[VulkanPipelineUseCase::Opaque] = false;
	this->minSampleShading_[VulkanPipelineUseCase::Opaque] = 0.0f;

	this->rasterizationSamples_[VulkanPipelineUseCase::Transparent] = vk::SampleCountFlagBits::e1;
	this->sampleShadingEnable_[VulkanPipelineUseCase::Transparent] = false;
	this->minSampleShading_[VulkanPipelineUseCase::Transparent] = 0.0f;
}

VulkanPipelineConfig::~VulkanPipelineConfig()
{
}

void VulkanPipelineConfig::SetMSAAProperties(vk::SampleCountFlagBits rasterizationSamples, bool sampleShadingEnable, float minSampleShading)
{
	this->rasterizationSamples_[VulkanPipelineUseCase::Opaque] = rasterizationSamples;
	this->sampleShadingEnable_[VulkanPipelineUseCase::Opaque] = sampleShadingEnable;
	this->minSampleShading_[VulkanPipelineUseCase::Opaque] = minSampleShading;

	this->rasterizationSamples_[VulkanPipelineUseCase::Transparent] = rasterizationSamples;
	this->sampleShadingEnable_[VulkanPipelineUseCase::Transparent] = sampleShadingEnable;
	this->minSampleShading_[VulkanPipelineUseCase::Transparent] = minSampleShading;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanBufferConfig::VulkanBufferConfig()
{
}

VulkanBufferConfig::~VulkanBufferConfig()
{
}

void VulkanBufferConfig::SetBufferSize(VulkanBufferUseCase bufferUseCase, vk::DeviceSize bufferSize)
{
	this->bufferSizes_[bufferUseCase] = bufferSize;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanImageConfig::VulkanImageConfig()
{
}

VulkanImageConfig::~VulkanImageConfig()
{
}

void VulkanImageConfig::SetImageSampleCount(VulkanImageUseCase imageUseCase, vk::SampleCountFlagBits imageSampleCount)
{
	this->imageSampleCounts_[imageUseCase] = imageSampleCount;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanCameraConfig::VulkanCameraConfig()
{
}

VulkanCameraConfig::~VulkanCameraConfig()
{
}

void VulkanCameraConfig::SetViewMatrixComponents(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 up, glm::vec3 upFallback)
{
	this->position_ = position;
	this->targetPosition_ = targetPosition;
	this->up_ = up;
	this->upFallback_ = upFallback;
}

void VulkanCameraConfig::SetPerspectiveMatrixComponents(float fieldOfView, float nearPlane, float farPlane)
{
	this->fieldOfView_ = fieldOfView;
	this->nearPlane_ = nearPlane;
	this->farPlane_ = farPlane;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanInputHandlerConfig::VulkanInputHandlerConfig()
{
}

VulkanInputHandlerConfig::~VulkanInputHandlerConfig()
{
}

void VulkanInputHandlerConfig::SetCursorIsCaptured(bool cursorIsCaptured)
{
	this->cursorIsCaptured_ = cursorIsCaptured;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

VulkanVisualizerConfig::VulkanVisualizerConfig()
{
}

VulkanVisualizerConfig::~VulkanVisualizerConfig()
{
}

void VulkanVisualizerConfig::SetLightSourceComponents(glm::vec3 lightSourcePosition, glm::vec3 lightSourceColor)
{
	this->internalConfig_.lightSourcePosition_ = glm::vec4(lightSourcePosition, 0.0f);
	this->internalConfig_.lightSourceColor_ = glm::vec4(lightSourceColor, 0.0f);
}

void VulkanVisualizerConfig::SetPhongParameters(bool usePhongReflectionModel, float phongAmbient, float phongShininess)
{
	this->internalConfig_.phongParameters_ = glm::vec4(static_cast<uint32_t>(usePhongReflectionModel), phongAmbient, phongShininess, 0.0f);
}

void VulkanVisualizerConfig::SetMaxVertexCount(size_t maxVertexCount)
{
	this->internalConfig_.maxVertexCount_ = maxVertexCount;

	if (this->internalConfig_.useIndexedDraw_ == true)
	{
		this->bufferConfig_.SetBufferSize(VulkanBufferUseCase::Vertex, maxVertexCount*sizeof(VulkanVertex));
		this->bufferConfig_.SetBufferSize(VulkanBufferUseCase::Index, maxVertexCount*sizeof(uint32_t));
		this->bufferConfig_.SetBufferSize(VulkanBufferUseCase::Staging, maxVertexCount*sizeof(VulkanVertex) + maxVertexCount*sizeof(uint32_t));
	}
	else
	{
		this->bufferConfig_.SetBufferSize(VulkanBufferUseCase::Vertex, maxVertexCount*sizeof(VulkanVertex));
		this->bufferConfig_.SetBufferSize(VulkanBufferUseCase::Staging, maxVertexCount*sizeof(VulkanVertex));
		this->bufferConfig_.SetBufferSize(VulkanBufferUseCase::Index, 1);
	}
}

void VulkanVisualizerConfig::SetFramesInFlightCount(size_t framesInFlightCount)
{
	this->internalConfig_.framesInFlightCount_ = framesInFlightCount;
}

void VulkanVisualizerConfig::UseIndexedDraw(bool useIndexedDraw)
{
	this->internalConfig_.useIndexedDraw_ = useIndexedDraw;

	if (useIndexedDraw == false)
	{
		// To make it simple, the index buffer is kept with 1B size.
		this->bufferConfig_.SetBufferSize(VulkanBufferUseCase::Index, 1);
	}
}

void VulkanVisualizerConfig::UseMSAA(bool useMSAA, vk::SampleCountFlagBits msaaSampleCount, bool enableSampleShading, float minSampleShading)
{
	this->internalConfig_.useMSAA_ = useMSAA;

	if (useMSAA == false || (msaaSampleCount & vk::SampleCountFlagBits::e1) == vk::SampleCountFlagBits::e1)
	{
		useMSAA = false;
		msaaSampleCount = vk::SampleCountFlagBits::e1;
		enableSampleShading = false;
		minSampleShading = 0.0f;
	}

	if (enableSampleShading == false)
	{
		minSampleShading = 0.0f;
	}

	this->deviceConfig_.SetMSAARequirements(msaaSampleCount, enableSampleShading);

	this->pipelineConfig_.SetMSAAProperties(msaaSampleCount, enableSampleShading, minSampleShading);

	this->imageConfig_.SetImageSampleCount(VulkanImageUseCase::Depth, msaaSampleCount);
	this->imageConfig_.SetImageSampleCount(VulkanImageUseCase::MSAA, msaaSampleCount);
}

void VulkanVisualizerConfig::SetShowLiveTimingInfo(bool showLiveTimingInfo)
{
	this->internalConfig_.showLiveTimingInfo_ = showLiveTimingInfo;
}

void VulkanVisualizerConfig::SetShowLiveFrameInfo(bool showLiveFrameInfo)
{
	this->internalConfig_.showLiveFrameInfo_= showLiveFrameInfo;
}

void VulkanVisualizerConfig::SetShowPostCloseTimingInfo(bool showPostCloseTimingInfo)
{
	this->internalConfig_.showPostCloseTimingInfo_ = showPostCloseTimingInfo;
}

void VulkanVisualizerConfig::SetVisualizerMode(bool is2D)
{
	this->internalConfig_.is2D_ = is2D;

	if (is2D == true)
	{
		// Camera is looking straight down at the origin.
		this->SetViewMatrixComponents_Camera({0.0f, 0.0f, 500.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
		this->SetPerspectiveMatrixComponents_Camera(0.0f, 0.01f, 1000.0f);

		// Lighting is disabled.
		this->SetPhongParameters(false, 0.0f, 0.0f);
	}
}

void VulkanVisualizerConfig::SetShowMetaInformation_Logger(bool showMetaInformation)
{
	this->loggerConfig_.SetShowMetaInformation(showMetaInformation);
}

void VulkanVisualizerConfig::SetRendererName(std::string name)
{
	this->windowConfig_.SetWindowName(name);
	this->instanceConfig_.SetApplicationName(name);
}

void VulkanVisualizerConfig::SetMinimumVulkanVersion(uint32_t minimumVulkanVersion)
{
	this->instanceConfig_.SetMinimumVulkanVersion(minimumVulkanVersion);
	this->deviceConfig_.SetMinimumVulkanVersion(minimumVulkanVersion);
	this->shaderModuleConfig_.SetMinimumVulkanVersion(minimumVulkanVersion);
}

void VulkanVisualizerConfig::SetWindowProperties_Window(VulkanWindowMode windowMode, int windowWidth, int windowHeight, int windowX, int windowY)
{
	if (windowMode == VulkanWindowMode::BorderlessFullscreen)
	{
		// Valid size is needed to create a window. Defaults will be used.
	}
	else if (windowMode == VulkanWindowMode::Window)
	{
		if (windowWidth <= 0 || windowHeight <= 0)
		{
			// Invalid size for window. Defaults are used.
		}
		else
		{
			this->windowConfig_.SetWindowWidth(windowWidth);
			this->windowConfig_.SetWindowHeight(windowHeight);
		}
	}

	this->windowConfig_.SetWindowMode(windowMode);
	this->windowConfig_.SetWindowX(windowX);
	this->windowConfig_.SetWindowY(windowY);
}

void VulkanVisualizerConfig::SetShowMessengerInfoMessages_Instance(bool showMessengerInfoMessages)
{
	this->instanceConfig_.SetShowMessengerInfoMessages(showMessengerInfoMessages);
}

void VulkanVisualizerConfig::SetShowMessengerVerboseMessages_Instance(bool showMessengerVerboseMessages)
{
	this->instanceConfig_.SetShowMessengerVerboseMessages(showMessengerVerboseMessages);
}

void VulkanVisualizerConfig::SetShowAvailableLayersAndExtensions_Instance(bool showAvailableLayersAndExtensions)
{
	this->instanceConfig_.SetShowAvailableLayersAndExtensions(showAvailableLayersAndExtensions);
}

void VulkanVisualizerConfig::SetEnableValidationLayers_Instance(bool enableValidationLayers)
{
	this->instanceConfig_.SetEnableValidationLayers(enableValidationLayers);
}

void VulkanVisualizerConfig::SetUseLayerSettings_Instance(bool useLayerSettings)
{
	this->instanceConfig_.SetUseLayerSettings(useLayerSettings);
}

void VulkanVisualizerConfig::SetShowAvailablePhysicalDevices_Device(bool showAvailablePhysicalDevices)
{
	this->deviceConfig_.SetShowAvailablePhysicalDevices(showAvailablePhysicalDevices);
}

void VulkanVisualizerConfig::SetCompileShaderAtRuntime_ShaderModule(bool compileShaderAtRuntime)
{
	this->shaderModuleConfig_.SetCompileShaderAtRuntime(compileShaderAtRuntime);
}

void VulkanVisualizerConfig::SetCompileShaderWithDebugFlags_ShaderModule(bool compileShaderWithDebugFlags)
{
	this->shaderModuleConfig_.SetCompileShaderWithDebugFlags(compileShaderWithDebugFlags);
}

void VulkanVisualizerConfig::SetBufferSize_Buffer(VulkanBufferUseCase bufferUseCase, vk::DeviceSize bufferSize)
{
	this->bufferConfig_.SetBufferSize(bufferUseCase, bufferSize);
}

void VulkanVisualizerConfig::SetViewMatrixComponents_Camera(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 up, glm::vec3 upFallback)
{
	this->cameraConfig_.SetViewMatrixComponents(position, targetPosition, up, upFallback);
}

void VulkanVisualizerConfig::SetPerspectiveMatrixComponents_Camera(float fieldOfView, float nearPlane, float farPlane)
{
	this->cameraConfig_.SetPerspectiveMatrixComponents(fieldOfView, nearPlane, farPlane);
}

void VulkanVisualizerConfig::SetCursorIsCaptured_InputHandler(bool cursorIsCaptured)
{
	this->inputHandlerConfig_.SetCursorIsCaptured(cursorIsCaptured);
}
