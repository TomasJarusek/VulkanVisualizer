
#include "VulkanInputHandler.h"

#include "VulkanConfig.h"
#include "VulkanWindow.h"


VulkanInputHandler::VulkanInputHandler(const VulkanWindow& window, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    window_(window)
{
    LOG_DBG(this->logger_) << "Creating input handler. "
        << "cursorIsCaptured <" << std::format("{}", visualizerConfig.inputHandlerConfig_.cursorIsCaptured_) << ">" << std::endl;

    this->rawMouseMotionSupported_ = glfwRawMouseMotionSupported();

    // Non-captured mode uses the operating system's cursor.
    // This results in the cursor having no latency, but things rendered
    // at its position will not be clamped to it.
    this->CreateCursor();

    this->SetCursorIsCaptured(visualizerConfig.inputHandlerConfig_.cursorIsCaptured_);

    LOG_DBG(this->logger_) << "Input handler created. " << std::endl;
}

VulkanInputHandler::~VulkanInputHandler()
{
    if (this->cursor_ != nullptr) 
    {
        glfwDestroyCursor(this->cursor_);
    }

    LOG_DBG(this->logger_) << "Input handler destroyed. " << std::endl;
}

void VulkanInputHandler::SetKeyState(int key, bool isDown, bool justPressed, bool justReleased)
{
    this->keyStates_[key].isDown_ = isDown;
    this->keyStates_[key].justPressed_ = justPressed;
    this->keyStates_[key].justReleased_ = justReleased;
}

void VulkanInputHandler::SetButtonState(int button, bool isDown, bool justPressed, bool justReleased)
{
    this->buttonStates_[button].isDown_ = isDown;
    this->buttonStates_[button].justPressed_ = justPressed;
    this->buttonStates_[button].justReleased_ = justReleased;
}

void VulkanInputHandler::SetCursor(double cursorX, double cursorY)
{
    this->cursorX_ = cursorX;
    this->cursorY_ = cursorY;
}

void VulkanInputHandler::UpdateScrollDelta(double scrollDeltaX, double scrollDeltaY)
{
    this->scrollDeltaX_ += scrollDeltaX;
    this->scrollDeltaY_ += scrollDeltaY;
}

void VulkanInputHandler::SetCursorIsCaptured(bool cursorIsCaptured)
{
    this->cursorIsCaptured_ = cursorIsCaptured;

    if (this->cursorIsCaptured_ == false)
    {
        // Standard visual cursor is set.

        glfwSetInputMode(this->window_.get().Get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursor(this->window_.get().Get(), this->cursor_);
    }
    else
    {
        // Captured cursor is set.

        // Default cursor disabled handling includes post-processing done by the OS (like acceleration).
        glfwSetInputMode(this->window_.get().Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (this->rawMouseMotionSupported_ == true)
        {
            // This uses raw mouse motion, which is better for captured mode.
            glfwSetInputMode(this->window_.get().Get(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }

    // Mouse position variables are initialized with the current mouse position.
    // glfwGetCursorPos() should bypass the event queue and query the OS directly.
    // This should prevent random warps and jumps during mode transition.
    glfwGetCursorPos(this->window_.get().Get(), &this->cursorX_, &this->cursorY_);
    this->lastCursorX_ = cursorX_;
    this->lastCursorY_ = cursorY_;
}

void VulkanInputHandler::CreateCursor()
{
    // Custom image could be used, for now, a standard cursor is created.
    this->cursor_ = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
}

void VulkanInputHandler::ResetKeyStates()
{
    // justPressed_ and justReleased_ can only be true for one frame.
    for (KeyButtonState& keyState : keyStates_)
    {
        keyState.justPressed_ = false;
        keyState.justReleased_ = false;
    }
}

void VulkanInputHandler::ResetButtonStates()
{
    // justPressed_ and justReleased_ can only be true for one frame.
    for (KeyButtonState& buttonState : buttonStates_)
    {
        buttonState.justPressed_ = false;
        buttonState.justReleased_ = false;
    }
}

void VulkanInputHandler::ResetScrollDelta()
{
    this->scrollDeltaX_ = 0.0;
    this->scrollDeltaY_ = 0.0;
}

void VulkanInputHandler::UpdateMouseState()
{
    this->cursorDeltaX_ = this->cursorX_ - this->lastCursorX_;
    this->cursorDeltaY_ = this->cursorY_ - this->lastCursorY_;

    this->lastCursorX_ = this->cursorX_;
    this->lastCursorY_ = this->cursorY_;
}

bool VulkanInputHandler::IsKeyDown(int glfwKey) const
{
    if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST)
    {
        return false;
    }
    else
    {
        return keyStates_[glfwKey].isDown_;
    }
}

bool VulkanInputHandler::IsKeyJustPressed(int glfwKey) const
{
    if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST)
    {
        return false;
    }
    else
    {
        return keyStates_[glfwKey].justPressed_;
    }
}

bool VulkanInputHandler::IsKeyJustReleased(int glfwKey) const
{
    if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST)
    {
        return false;
    }
    else
    {
        return keyStates_[glfwKey].justReleased_;
    }
}

void VulkanInputHandler::GetCursorPos(double& cursorX, double& cursorY) const
{
    auto [width, height] = this->window_.get().GetWindowSize();

    cursorX = this->cursorX_ - static_cast<double>(width)/2.0;
    cursorY = static_cast<double>(height)/2.0 - this->cursorY_;
}

void VulkanInputHandler::GetCursorDelta(double& cursorDeltaX, double& cursorDeltaY) const
{
    cursorDeltaX = this->cursorDeltaX_;
    cursorDeltaY = this->cursorDeltaY_;
}

bool VulkanInputHandler::IsButtonDown(int glfwButton) const
{
    if (glfwButton < 0 || glfwButton > GLFW_MOUSE_BUTTON_LAST)
    {
        return false;
    }
    else
    {
        return buttonStates_[glfwButton].isDown_;
    }
}

bool VulkanInputHandler::IsButtonJustPressed(int glfwButton) const
{
    if (glfwButton < 0 || glfwButton > GLFW_MOUSE_BUTTON_LAST)
    {
        return false;
    }
    else
    {
        return buttonStates_[glfwButton].justPressed_;
    }
}

bool VulkanInputHandler::IsButtonJustReleased(int glfwButton) const
{
    if (glfwButton < 0 || glfwButton > GLFW_MOUSE_BUTTON_LAST)
    {
        return false;
    }
    else
    {
        return buttonStates_[glfwButton].justReleased_;
    }
}

void VulkanInputHandler::GetScrollDelta(double& scrollDeltaX, double& scrollDeltaY) const
{
    scrollDeltaX = this->scrollDeltaX_;
    scrollDeltaY = this->scrollDeltaY_;
}
