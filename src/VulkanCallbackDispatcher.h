
#pragma once

#include <GLFW/glfw3.h>

#include "VulkanWindow.h"
#include "VulkanInputHandler.h"

class VulkanCallbackDispatcher
{
private:

    // Direct references are used here instead of reference wrappers.
    // Both the window and the input handler will exist for the lifetime of the application.
    // Potentially lower overhead during a callback invocation.
    VulkanWindow& window_;
    VulkanInputHandler& inputHandler_;

public:

    // Constructor.
    VulkanCallbackDispatcher(VulkanWindow& window, VulkanInputHandler& inputHandler) :
        window_(window),
        inputHandler_(inputHandler)
    {
        // GLFW stores dispatcher pointer.
        glfwSetWindowUserPointer(window.Get() , this);

        // Callback handlers are set.
        glfwSetFramebufferSizeCallback(window.Get(), VulkanCallbackDispatcher::FramebufferSizeCallbackHandler);
        glfwSetKeyCallback(window.Get(), VulkanCallbackDispatcher::KeyCallbackHandler);
        glfwSetCursorPosCallback(window.Get(), VulkanCallbackDispatcher::CursorPosCallbackHandler);
        glfwSetMouseButtonCallback(window.Get(), VulkanCallbackDispatcher::ButtonCallbackHandler);
        glfwSetScrollCallback(window.Get(), VulkanCallbackDispatcher::ScrollCallbackHandler);
    }

    // Destructor.
    ~VulkanCallbackDispatcher()
    {
        glfwSetScrollCallback(this->window_.Get(), nullptr);
        glfwSetMouseButtonCallback(this->window_.Get(), nullptr);
        glfwSetCursorPosCallback(this->window_.Get(), nullptr);
        glfwSetKeyCallback(this->window_.Get(), nullptr);
        glfwSetFramebufferSizeCallback(this->window_.Get(), nullptr);

        glfwSetWindowUserPointer(this->window_.Get(), nullptr);
    }

    // Copy/move constructor/assignment.
    VulkanCallbackDispatcher(const VulkanCallbackDispatcher& callbackDispatcher) = delete;
    VulkanCallbackDispatcher(VulkanCallbackDispatcher&& callbackDispatcher) noexcept = delete;
    VulkanCallbackDispatcher& operator=(const VulkanCallbackDispatcher& callbackDispatcher) = delete;
    VulkanCallbackDispatcher& operator=(VulkanCallbackDispatcher&& callbackDispatcher) noexcept = delete;

private:

    static void FramebufferSizeCallbackHandler(GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height)
    {
        VulkanCallbackDispatcher* dispatcherUserPointer = reinterpret_cast<VulkanCallbackDispatcher*>(glfwGetWindowUserPointer(window));
        dispatcherUserPointer->window_.SetFramebufferResized(true);
    }

    static void KeyCallbackHandler(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
    {
        if (key >= 0 && key <= GLFW_KEY_LAST)
        {
            VulkanCallbackDispatcher* dispatcherUserPointer = reinterpret_cast<VulkanCallbackDispatcher*>(glfwGetWindowUserPointer(window));

            if (action == GLFW_PRESS) 
            {
                dispatcherUserPointer->inputHandler_.SetKeyState(key, true, true, false);
            }
            else if (action == GLFW_RELEASE) 
            {
                dispatcherUserPointer->inputHandler_.SetKeyState(key, false, false, true);
            }
        }
    }

    static void CursorPosCallbackHandler(GLFWwindow* window, double xpos, double ypos)
    {
        VulkanCallbackDispatcher* dispatcherUserPointer = reinterpret_cast<VulkanCallbackDispatcher*>(glfwGetWindowUserPointer(window));

        dispatcherUserPointer->inputHandler_.SetCursor(xpos, ypos);
    }

    static void ButtonCallbackHandler(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
    {
        if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST)
        {
            VulkanCallbackDispatcher* dispatcherUserPointer = reinterpret_cast<VulkanCallbackDispatcher*>(glfwGetWindowUserPointer(window));

            if (action == GLFW_PRESS) 
            {
                dispatcherUserPointer->inputHandler_.SetButtonState(button, true, true, false);
            }
            else if (action == GLFW_RELEASE) 
            {
                dispatcherUserPointer->inputHandler_.SetButtonState(button, false, false, true);
            }
        }
    }

    static void ScrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset)
    {
        VulkanCallbackDispatcher* dispatcherUserPointer = reinterpret_cast<VulkanCallbackDispatcher*>(glfwGetWindowUserPointer(window));

        dispatcherUserPointer->inputHandler_.UpdateScrollDelta(xoffset, yoffset);
    }
};
