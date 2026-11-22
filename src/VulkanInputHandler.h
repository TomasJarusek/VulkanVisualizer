
#pragma once

#include <GLFW/glfw3.h>

#include <array>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanWindow;


class VulkanInputHandler
{
private:

    Logger logger_;

    // Window reference.
    std::reference_wrapper<const VulkanWindow> window_;

    // Key/button state.
    struct KeyButtonState 
    {
        bool isDown_ = false;
        bool justPressed_ = false;
        bool justReleased_ = false;
    };

    // Keyboard.
    std::array<KeyButtonState, GLFW_KEY_LAST + 1> keyStates_{};

    // Mouse.
    std::array<KeyButtonState, GLFW_MOUSE_BUTTON_LAST + 1> buttonStates_{};

    GLFWcursor* cursor_ = nullptr;

    bool cursorIsCaptured_ = false;

    bool rawMouseMotionSupported_ = false;

    double cursorX_ = 0.0;
    double cursorY_ = 0.0;
    double lastCursorX_ = 0.0;
    double lastCursorY_ = 0.0;
    double cursorDeltaX_ = 0.0;
    double cursorDeltaY_ = 0.0;

    // Accumulated total scroll delta over the course of the frame.
    double scrollDeltaX_ = 0.0;
    double scrollDeltaY_ = 0.0;

public:

    VulkanInputHandler(const VulkanWindow& window, const VulkanVisualizerConfig& visualizerConfig);
    ~VulkanInputHandler();

    void SetKeyState(int key, bool isDown, bool justPressed, bool justReleased);
    void SetButtonState(int button, bool isDown, bool justPressed, bool justReleased);

    void SetCursor(double cursorX, double cursorY);

    void UpdateScrollDelta(double scrollDeltaX, double scrollDeltaY);

    // Switches mouse mode between cursor and captured.
    void SetCursorIsCaptured(bool cursorIsCaptured);

    // Resets all key states. Must be called before glfwPollEvents().
    void ResetKeyStates();
    // Resets all button states. Must be called before glfwPollEvents().
    void ResetButtonStates();
    // Resets scroll delta. Must be called before glfwPollEvents().
    void ResetScrollDelta();

    // Updates mouse related variables. Must be called after glfwPollEvents().
    void UpdateMouseState();

    // Keyboard queries.
    bool IsKeyDown(int glfwKey) const;
    bool IsKeyJustPressed(int glfwKey) const;
    bool IsKeyJustReleased(int glfwKey) const;

    // Mouse queries.
    void GetCursorPos(double& cursorX, double& cursorY) const;
    void GetCursorDelta(double& cursorDeltaX, double& cursorDeltaY) const;
    bool IsButtonDown(int glfwButton) const;
    bool IsButtonJustPressed(int glfwButton) const;
    bool IsButtonJustReleased(int glfwButton) const;
    void GetScrollDelta(double& scrollDeltaX, double& scrollDeltaY) const;

private:

    void CreateCursor();
};
