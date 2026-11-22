
#include "VulkanWindow.h"

#include <limits>

#include "VulkanConfig.h"
#include "VulkanHelperFunctions.h"


VulkanWindow::VulkanWindow(const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    glfwWindowName_(visualizerConfig.windowConfig_.windowName_),
    glfwWindowMode_(visualizerConfig.windowConfig_.windowMode_),
    glfwWindowWidth_(visualizerConfig.windowConfig_.windowWidth_),
    glfwWindowHeight_(visualizerConfig.windowConfig_.windowHeight_),
    glfwWindowX_(visualizerConfig.windowConfig_.windowX_),
    glfwWindowY_(visualizerConfig.windowConfig_.windowY_)
{
    LOG_DBG(this->logger_) << "Creating window. "
        << "glfwWindowName <" << this->glfwWindowName_ << ">, "
        << "glfwWindowMode <" << GetPrintableValue(this->glfwWindowMode_) << ">, "
        << "glfwWindowWidth <" << this->glfwWindowWidth_ << ">, "
        << "glfwWindowHeight <" << this->glfwWindowHeight_ << ">, "
        << "glfwWindowX <" << this->glfwWindowX_ << ">, "
        << "glfwWindowY <" << this->glfwWindowY_ << ">" << std::endl;

    this->CreateGLFWWindow();

    LOG_DBG(this->logger_) << "Window created." << std::endl;
}

VulkanWindow::~VulkanWindow()
{
    void DestroyGLFWWindow();

    LOG_DBG(this->logger_) << "Window destroyed." << std::endl;
}

GLFWwindow* VulkanWindow::Get() const
{
    return this->glfwWindow_;
}

void VulkanWindow::SetWindowMode(VulkanWindowMode windowMode, int windowWidth, int windowHeight, int windowX, int windowY)
{
    if (windowMode == VulkanWindowMode::Window)
    {
        if (windowWidth <= 0 || windowHeight <= 0)
        {
            // Defaults to currently stored values.

            windowWidth = this->glfwWindowWidth_;
            windowHeight = this->glfwWindowHeight_;
        }
        else
        {
            // Values are valid, so they are stored as current values.

            this->glfwWindowWidth_ = windowWidth;
            this->glfwWindowHeight_ = windowHeight;
        }

        if (windowX == std::numeric_limits<int>::min() || windowY == std::numeric_limits<int>::min())
        {
            // Defaults to currently stored values.

            windowX = this->glfwWindowX_;
            windowY = this->glfwWindowY_;
        }
        else
        {
            // Values are valid, so they are stored as current values.

            this->glfwWindowX_ = windowX;
            this->glfwWindowY_ = windowY;
        }

        // Standard operating system decorations are enabled.
        glfwSetWindowAttrib(this->glfwWindow_, GLFW_DECORATED, GLFW_TRUE);
        // Window does not stay above other windows.
        glfwSetWindowAttrib(this->glfwWindow_, GLFW_FLOATING, GLFW_FALSE);

        // Window properties are set. Refresh rate is not chosen explicitly.
        glfwSetWindowMonitor(this->glfwWindow_, nullptr, windowX, windowY, windowWidth, windowHeight, GLFW_DONT_CARE);
    }
    else if (windowMode == VulkanWindowMode::BorderlessFullscreen)
    {
        // Primary monitor and its mode are retrieved.
        GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* glfwVidmode = glfwGetVideoMode(glfwMonitor);

        auto [currentMonitorX, currentMonitorY] = this->GetPrimaryMonitorPosition();

        if (this->glfwWindowMode_ == VulkanWindowMode::Window)
        {
            // If the previous mode was window, we store its properties so they can be restored if fullscreen is exited.

            auto [currentWindowX, currentWindowY] = this->GetWindowPosition();
            auto [currentWindowWidth, currentWindowHeight] = this->GetWindowSize();

            this->glfwWindowX_ = currentWindowX;
            this->glfwWindowY_ = currentWindowY;

            this->glfwWindowWidth_ = currentWindowWidth;
            this->glfwWindowHeight_ = currentWindowHeight;
        }

        // Prevents a window flicker during transition.
        glfwHideWindow(this->glfwWindow_);

        // Strip OS title bar/borders and prevent minimizing when clicking into other windows.
        glfwSetWindowAttrib(this->glfwWindow_, GLFW_DECORATED, GLFW_FALSE);
        glfwSetWindowAttrib(this->glfwWindow_, GLFW_AUTO_ICONIFY, GLFW_FALSE);
        glfwSetWindowAttrib(this->glfwWindow_, GLFW_FLOATING, GLFW_FALSE);

        // Pass nullptr for the monitor to keep it in true windowed mode at exact monitor dimensions.
        glfwSetWindowMonitor(this->glfwWindow_, nullptr, currentMonitorX, currentMonitorY, glfwVidmode->width, glfwVidmode->height, GLFW_DONT_CARE);

        // Restores and focuses window.
        glfwShowWindow(this->glfwWindow_);
        glfwFocusWindow(this->glfwWindow_);
    }
    // To get a proper exclusive fullscreen with a G-SYNC support,
    // a VK_EXT_full_screen_exclusive would have to be used.
    // The problem is that a native Vulkan swapchain is not supported by default.
    else if (windowMode == VulkanWindowMode::ExclusiveFullscreen)
    {
        // Primary monitor and its mode are retrieved.
        GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* glfwVidmode = glfwGetVideoMode(glfwMonitor);

        auto [currentMonitorX, currentMonitorY] = this->GetPrimaryMonitorPosition();

        if (this->glfwWindowMode_ == VulkanWindowMode::Window)
        {
            // If the previous mode was window, we store its properties so they can be restored if fullscreen is exited.

            auto [currentWindowX, currentWindowY] = this->GetWindowPosition();
            auto [currentWindowWidth, currentWindowHeight] = this->GetWindowSize();

            this->glfwWindowX_ = currentWindowX;
            this->glfwWindowY_ = currentWindowY;

            this->glfwWindowWidth_ = currentWindowWidth;
            this->glfwWindowHeight_ = currentWindowHeight;
        }

        // Prevents a window flicker during a transition to fullscreen.
        glfwHideWindow(this->glfwWindow_);

        // Reenable auto-minimize on focus loss (in case we switched from BorderlessFullscreen).
        glfwSetWindowAttrib(this->glfwWindow_, GLFW_AUTO_ICONIFY, GLFW_TRUE);

        // Monitor is explicitly set together with window properties.
        glfwSetWindowMonitor(this->glfwWindow_, glfwMonitor, currentMonitorX, currentMonitorY, glfwVidmode->width, glfwVidmode->height, glfwVidmode->refreshRate);
        
        // Restores window.
        glfwShowWindow(this->glfwWindow_);
        glfwFocusWindow(this->glfwWindow_);
    }

    this->glfwWindowMode_ = windowMode;
}

void VulkanWindow::SetFramebufferResized(bool framebufferResized)
{
    this->framebufferResized_ = framebufferResized;
}

bool VulkanWindow::GetFramebufferResized() const
{
    return this->framebufferResized_;
}

std::pair<int, int> VulkanWindow::GetWindowSize() const
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(this->glfwWindow_, &width, &height);

    return {width, height};
}

std::pair<int, int> VulkanWindow::GetFramebufferSize() const
{
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(this->glfwWindow_, &width, &height);

    return {width, height};
}

std::pair<int, int> VulkanWindow::GetWindowPosition() const
{
    int windowX = 0;
    int windowY = 0;

    glfwGetWindowPos(this->glfwWindow_, &windowX, &windowY);

    return {windowX, windowY};
}

std::pair<int, int> VulkanWindow::GetPrimaryMonitorPosition() const
{
    GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();

    int monitorX = 0;
    int monitorY = 0;

    glfwGetMonitorPos(glfwMonitor, &monitorX, &monitorY);

    return {monitorX, monitorY};
}

bool VulkanWindow::WindowShouldClose() const
{
    return static_cast<bool>(glfwWindowShouldClose(this->glfwWindow_));
}

void VulkanWindow::HandleMinimization() const
{
    // Get current window size.
    auto [width, height] = this->GetFramebufferSize();

    // If the size is zero, the window is minimized.
    while (width == 0 || height == 0)
    {
        // Active wait until the window is not minimized anymore.

        // Check OS events.
        // glfwPollEvents() could be used here instead, but that would cause an unconstrained loop.
        // glfwWaitEvents() does the same thing, but waits until there are new events to process in the OS queue.
        glfwWaitEvents();

        // After the OS events are processed, the window size is rechecked.
        glfwGetFramebufferSize(this->glfwWindow_, &width, &height);
    }
}

std::pair<int, int> VulkanWindow::CalculateCenteredWindowCoordinates(int windowWidth, int windowHeight) const
{
    // Primary monitor and its mode are retrieved.
    GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* glfwVidmode = glfwGetVideoMode(glfwMonitor);

    // Monitor position.
    int monitorX = 0;
    int monitorY = 0;
    glfwGetMonitorPos(glfwMonitor, &monitorX, &monitorY);

    return {monitorX + (glfwVidmode->width - windowWidth)/2, monitorY + (glfwVidmode->height - windowHeight)/2};
}

void VulkanWindow::CreateGLFWWindow()
{
    if (glfwInit() == GLFW_FALSE)
    {
        throw std::runtime_error("Exception: GLFW could not be initialized.");
    }

    // Vulkan support check.
    if (glfwVulkanSupported() != GLFW_TRUE)
    {
        glfwTerminate();

        throw std::runtime_error("Exception: Vulkan is not supported in this GLFW version.");
    }


    // Window/Fullscreen setting.
    GLFWmonitor* glfwMonitor = nullptr;
    int createWindowWidth = this->glfwWindowWidth_;
    int createWindowHeight = this->glfwWindowHeight_;

    if (this->glfwWindowMode_ == VulkanWindowMode::Window)
    {
        // Initial flash of an empty window could be resolved by hiding/showing the window for the necessary period. Fine for now.
    }
    if (this->glfwWindowMode_ == VulkanWindowMode::BorderlessFullscreen)
    {
        // Primary monitor and its mode are retrieved.
        glfwMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* glfwVidmode = glfwGetVideoMode(glfwMonitor);

        createWindowWidth = glfwVidmode->width;
        createWindowHeight = glfwVidmode->height;

        // Strip OS title bar/borders and prevent minimizing when clicking into other windows.
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

        // Not given explicitly in borderless window mode.
        glfwMonitor = nullptr;
    }
    else if (this->glfwWindowMode_ == VulkanWindowMode::ExclusiveFullscreen)
    {
        // Primary monitor and its mode are retrieved.
        glfwMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* glfwVidmode = glfwGetVideoMode(glfwMonitor);

        createWindowWidth = glfwVidmode->width;
        createWindowHeight = glfwVidmode->height;

        glfwWindowHint(GLFW_REFRESH_RATE, glfwVidmode->refreshRate);
    }


    // Specifies which client API to create.
    // Originally designed to create an OpenGL context, so for Vulkan, we need to specify to not create any.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    this->glfwWindow_ = glfwCreateWindow(createWindowWidth, createWindowHeight, this->glfwWindowName_.c_str(), glfwMonitor, nullptr);

    if (this->glfwWindow_ == nullptr)
    {
        glfwTerminate();

        throw std::runtime_error("Exception: GLFW window could not be created.");
    }

    // Window is centered if no valid coordinates are given.
    if (this->glfwWindowX_ == std::numeric_limits<int>::min() || this->glfwWindowY_ == std::numeric_limits<int>::min())
    {
        auto [calculatedWindowX, calculatedWindowY] = this->CalculateCenteredWindowCoordinates(this->glfwWindowWidth_, this->glfwWindowHeight_);

        this->glfwWindowX_ = calculatedWindowX;
        this->glfwWindowY_ = calculatedWindowY;
    }

    // Window position.
    if (this->glfwWindowMode_ == VulkanWindowMode::Window)
    {
        glfwSetWindowPos(this->glfwWindow_, this->glfwWindowX_, this->glfwWindowY_);
    }
    // Borderless window position.
    else if (this->glfwWindowMode_ == VulkanWindowMode::BorderlessFullscreen)
    {
        auto [monitorX, monitorY] = this->GetPrimaryMonitorPosition();

        glfwSetWindowPos(this->glfwWindow_, monitorX, monitorY);
    }
}

void VulkanWindow::DestroyGLFWWindow()
{
    // GLFW window destruction. Opposite of glfwCreateWindow().
    if (this->glfwWindow_ != nullptr)
    {
        glfwDestroyWindow(this->glfwWindow_);
    }

    // Termination of GLFW. Opposite of glfwInit().
    glfwTerminate();
}
