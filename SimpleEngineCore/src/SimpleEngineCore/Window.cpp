#include "SimpleEngineCore/Window.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>



namespace SimpleEngine {

    Window::Window(std::string title, const unsigned int width, const unsigned int height)
        : m_data({ std::move(title), width, height }) {

        int resultCode = init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
    }

    void Window::on_update() {

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

    int Window::init() {

        std::cout << " Window::init()\n";

        glfwSetErrorCallback([](int error_code, const char* description) {

                std::cerr << "GLFW error: " << description << "\n";
            });

        if (!glfwInit()) {
            std::cerr << "Can't initialize GLFW!\n";
            return -1;
        }

        /* Create a windowed mode window and its OpenGL context */
        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
        if (!m_pWindow) {

            std::cerr << "Can't create WINDOW!\n";
            return -2;
        }

        if (!Renderer_OpenGL::init(m_pWindow)) {

            std::cerr << "Failed to initialize Renderer_OpenGL" << "\n";
            return -3;
        }

        //Вызываем glfwSetWindowUserPointer, чтобы поймать пользовательские данные
        glfwSetWindowUserPointer(m_pWindow, &m_data);
        
        glfwSetKeyCallback(m_pWindow,
            [](GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
                
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                switch (action) {
                    case GLFW_PRESS: {
                        EventKeyPressed event(static_cast<KeyCode>(key), false);
                        data.eventCallbackFn(event);
                        //std::cout << "GLFW_PRESS: " << (char)key << "\n";
                        break;
                    }
                    case GLFW_RELEASE: {
                        EventKeyReleased event(static_cast<KeyCode>(key));
                        data.eventCallbackFn(event);
                        //std::cout << "GLFW_RELEASE\n";
                        break;
                    }
                    case GLFW_REPEAT: {
                        EventKeyPressed event(static_cast<KeyCode>(key), true);
                        data.eventCallbackFn(event);
                        //std::cout << "GLFW_REPEAT\n";
                        break;
                    }
                }
            });

        glfwSetWindowSizeCallback(m_pWindow,
            //Вызываем лямбда-функцию
            [](GLFWwindow* pWindow, int width, int height) {

                //std::cout << "New size " << width << "x" << height << "\n";
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                data.width = width;
                data.height = height;
                EventWindowResize event(width, height);
                data.eventCallbackFn(event);
            }
            );

        glfwSetCursorPosCallback(m_pWindow,
            [](GLFWwindow* pWindow, double x, double y) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                EventMouseMoved event(x, y);
                data.eventCallbackFn(event);
            });

        glfwSetWindowCloseCallback(m_pWindow,
            [](GLFWwindow* pWindow) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                EventWindowClose event;
                data.eventCallbackFn(event);
            });

        glfwSetFramebufferSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height) {

                Renderer_OpenGL::set_viewport(width, height);
            });

        return 0;
    }

    void Window::shutdown() {

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }

    Window::~Window() {
        shutdown();
    }
}