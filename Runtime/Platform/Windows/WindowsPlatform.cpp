#include "WindowsPlatform.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../../InputEvents.h"

static void error_callback(int error, const char *description)
{
    //LOGE("GLFW Error (code {}): {}", error, description);
}

static void window_close_callback(GLFWwindow *window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void window_size_callback(GLFWwindow *window, int width, int height)
{

}

static void window_focus_callback(GLFWwindow *window, int focused)
{

}

static inline KeyCode translate_key_code(int key)
{
    return KeyCode::Unknown;
}

static inline KeyAction translate_key_action(int action)
{
    return KeyAction::Unknown;
}

static void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/)
{
}

static inline MouseButton translate_mouse_button(int button)
{
    return MouseButton::Unknown;
}

static inline MouseAction translate_mouse_action(int action)
{
    return MouseAction::Unknown;
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int /*mods*/)
{
}
WindowsPlatform::WindowsPlatform(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{

}

WindowsPlatform::~WindowsPlatform()
{

}

bool WindowsPlatform::Initialize(std::unique_ptr<Application>&& application)
{
    if (!glfwInit())
    {
        //LogError("GLFW couldn't be initialized.");
        return false;
    }
    m_CurrentApplication = std::move(application);

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    //TODO
    auto width = 1024;
    auto height = 768;
    m_Handle = glfwCreateWindow(width, height, m_CurrentApplication->GetName().c_str(), NULL, NULL);
    if (!m_Handle)
    {
        //LogError("Couldn't create glfw window.");
        return false;
    }

    glfwSetWindowUserPointer(m_Handle, this);

    glfwSetWindowCloseCallback(m_Handle, window_close_callback);
    glfwSetWindowSizeCallback(m_Handle, window_size_callback);
    glfwSetWindowFocusCallback(m_Handle, window_focus_callback);
    glfwSetKeyCallback(m_Handle, key_callback);
    glfwSetCursorPosCallback(m_Handle, cursor_position_callback);
    glfwSetMouseButtonCallback(m_Handle, mouse_button_callback);

    glfwSetInputMode(m_Handle, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(m_Handle, GLFW_STICKY_MOUSE_BUTTONS, 1);

    return true;
}

void WindowsPlatform::MainLoop()
{
    while (!ShouldClose())
    {
        Tick();
        ProcesEvents();
    }
}

void WindowsPlatform::Terminate()
{
    glfwDestroyWindow(m_Handle);
    glfwTerminate();
}

bool WindowsPlatform::ShouldClose()
{
    return glfwWindowShouldClose(m_Handle);
}

void WindowsPlatform::ProcesEvents()
{
    glfwPollEvents();
}

void WindowsPlatform::Tick()
{

}