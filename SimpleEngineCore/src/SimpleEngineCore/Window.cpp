#include "SimpleEngineCore/Window.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace SimpleEngine {

    static bool s_GLFW_initialized = false;

    GLfloat points[] = {
         0.0f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    std::unique_ptr<ShaderProgram> p_shader_program;
    std::unique_ptr<VertexBuffer> p_points_vbo;
    std::unique_ptr<VertexBuffer> p_colors_vbo;
    std::unique_ptr<VertexArray> p_vao;
    //GLuint vao;

    //Шейдкры пишутся на языке ява селл
    const char* vertex_shader =
        "#version 460\n"
        "layout(location = 0) in vec3 vertex_position;"
        "layout(location = 1) in vec3 vertex_color;"
        "out vec3 color;"
        "void main() {"
        "    color = vertex_color;"
        "    gl_Position = vec4(vertex_position, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 460\n"
        "in vec3 color;"
        "out vec4 frag_color;"
        "void main() {"
        "    frag_color = vec4(color, 1.0);"
        "}";

    Window::Window(std::string title, const unsigned int width, const unsigned int height)
        : m_data({ std::move(title), width, height }) {

        int resultCode = init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
    }

    void Window::on_update() {

        glClearColor(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //Отрисовка треугольника
        p_shader_program->bind();
        //glUseProgram(shader_program);
        //glBindVertexArray(vao);
        p_vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);




        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(get_width());
        io.DisplaySize.y = static_cast<float>(get_height());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        ImGui::Begin("Background Color Window");
        ImGui::ColorEdit4("Background Color", m_background_color);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

        //Инициализируем ГЛАД
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << "\n";
            return -3;
        }

        //Вызываем glfwSetWindowUserPointer, чтобы поймать пользовательские данные
        glfwSetWindowUserPointer(m_pWindow, &m_data);
        
        //Ловим Event, который происходит при изменении окна, но не имеем доступ к классу Window
        //поэтому выше вызываем glfwSetWindowUserPointer
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

        //Рисуем треугольник
        glfwSetFramebufferSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height) {

                glViewport(0, 0, width, height);
            });

        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->isCompiled())
            return false;

        /*GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_shader, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_shader, nullptr);
        glCompileShader(fs);

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vs);
        glAttachShader(shader_program, fs);
        glLinkProgram(shader_program);

        glDeleteShader(vs);
        glDeleteShader(fs);*/

        /*GLuint points_vbo = 0;
        glGenBuffers(1, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);*/

        p_points_vbo = std::make_unique<VertexBuffer>(points, sizeof(points));

        /*GLuint colors_vbo = 0;
        glGenBuffers(1, &colors_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);*/

        p_colors_vbo = std::make_unique<VertexBuffer>(colors, sizeof(colors));

        //glGenVertexArrays(1, &vao);
        //glBindVertexArray(vao);

        //glEnableVertexAttribArray(0);
        ////glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        //p_points_vbo->bind();
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);//Привязываем к layout(location = 0)

        //glEnableVertexAttribArray(1);
        ////glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        //p_colors_vbo->bind();
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);//Привязываем к layout(location = 1)

        p_vao = std::make_unique<VertexArray>();
        p_vao->add_buffer(*p_points_vbo);
        p_vao->add_buffer(*p_colors_vbo);

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