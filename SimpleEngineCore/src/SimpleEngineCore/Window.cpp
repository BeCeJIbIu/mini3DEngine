#include "SimpleEngineCore/Window.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace SimpleEngine {

    static bool s_GLFW_initialized = false;

    Window::Window(std::string title, const unsigned int width, const unsigned int height)
        : m_data({ std::move(title), width, height }) {

        int resultCode = init();
    }

    void Window::on_update() {

        glClearColor(0.5, 0.5, 0, 0);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(m_pWindow);

        /* Poll for and process events */
        glfwPollEvents();
    }

    int Window::init() {

        std::cout << " Window::init()\n";

        /* Initialize the library */
        if (!s_GLFW_initialized) {
            if (!glfwInit()) {
                std::cerr << "Can't initialize GLFW!\n";
                return -1;
            }
            s_GLFW_initialized = true;
        }
        

        /* Create a windowed mode window and its OpenGL context */
        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
        if (!m_pWindow)
        {
            std::cerr << "Can't create WINDOW!\n";
            glfwTerminate();
            return -2;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(m_pWindow);

        //�������������� ����
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << "\n";
            return -3;
        }

        //�������� glfwSetWindowUserPointer, ����� ������� ���������������� ������
        glfwSetWindowUserPointer(m_pWindow, &m_data);
        
        //����� Event, ������� ���������� ��� ��������� ����, �� �� ����� ������ � ������ Window
        //������� ���� �������� glfwSetWindowUserPointer
        glfwSetWindowSizeCallback(m_pWindow,
            //�������� �����-�������
            [](GLFWwindow* pWindow, int width, int height) {

                //std::cout << "New size " << width << "x" << height << "\n";
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                data.width = width;
                data.height = height;

                Event event;
                event.width = width;
                event.height = height;
                data.eventCallbackFn(event);
            }
            );

        return 0;
    }

    void Window::shutdown() {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }

    Window::~Window() {
        shutdown();
    }
}