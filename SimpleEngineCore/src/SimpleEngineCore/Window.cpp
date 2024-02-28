#include "SimpleEngineCore/Window.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <glm/mat3x3.hpp>
#include <glm/trigonometric.hpp>

namespace SimpleEngine {

    static bool s_GLFW_initialized = false;

    //GLfloat positions_colors[] = {
    //    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
    //     0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
    //    -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,

    //     0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
    //    -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
    //     0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f
    //};

    GLfloat positions_colors2[] = {
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f
    };

    GLuint indexes[] = {
        0, 1, 2, 3, 2, 1
    };

    //������� ������� �� ����� ��� ����
    //const char* vertex_shader =
    //    R"(#version 460
    //    layout(location = 0) in vec3 vertex_position;
    //    layout(location = 1) in vec3 vertex_color;
    //    #��� �������������
    //    uniform mat4 scale_matrix;
    //    #��� ��������
    //    uniform mat4 rotate_matrix;
    //    #��� �����������
    //    uniform mat4 translate_matrix;
    //    out vec3 color;
    //    void main() {
    //        color = vertex_color;
    //        gl_Position = translate_matrix * rotate_matrix * scale_matrix * vec4(vertex_position, 1.0);
    //    })";

    const char* vertex_shader =
        R"(#version 460
        layout(location = 0) in vec3 vertex_position;
        layout(location = 1) in vec3 vertex_color;
        #����� ������� ��� ���� ����� ��������� �������
        uniform mat4 model_matrix;
        out vec3 color;
        void main() {
            color = vertex_color;
            gl_Position = model_matrix * vec4(vertex_position, 1.0);
        })";

    const char* fragment_shader =
        R"(#version 460
        in vec3 color;
        out vec4 frag_color;
        void main() {
            frag_color = vec4(color, 1.0);
        })";

    std::unique_ptr<ShaderProgram> p_shader_program;
    std::unique_ptr<VertexBuffer> p_positions_colors_vbo;
    std::unique_ptr<IndexBuffer> p_index_buffer;
    std::unique_ptr<VertexArray> p_vao;

    float scale[3] = { 1.f, 1.f, 1.f };
    float rotate = 0.f;
    float translate[3] = { 0.f, 0.f, 1.f };

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

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(get_width());
        io.DisplaySize.y = static_cast<float>(get_height());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow();

        ImGui::Begin("Background Color Window");
        ImGui::ColorEdit4("Background Color", m_background_color);
        ImGui::SliderFloat3("scale", scale, 0.f, 2.f);
        ImGui::SliderFloat("rotate", &rotate, 0.f, 360.f);
        ImGui::SliderFloat3("translate", translate, -1.f, 1.f);

        //static bool use_2_buffers = true;
        //ImGui::Checkbox("2 Buffers", &use_2_buffers);
        //if (use_2_buffers)
        //    p_vao_2buffers->bind();//��� 2 �������

        p_shader_program->bind();

        glm::mat4 scale_matrix( scale[0],   0,          0,          0,
                                0,          scale[1],   0,          0,
                                0,          0,          scale[2],   0,
                                0,          0,          0,          1);

        float rotate_in_radians = glm::radians(rotate);
        glm::mat4 rotate_matrix( cos(rotate_in_radians), sin(rotate_in_radians), 0, 0,
                                -sin(rotate_in_radians), cos(rotate_in_radians), 0, 0,
                                 0,                      0,                      1, 0,
                                 0,                      0,                      0, 1);

        glm::mat4 translate_matrix( 1,            0,            0,            0,
                                    0,            1,            0,            0,
                                    0,            0,            1,            0,
                                    translate[0], translate[1], translate[2], 1);

        //p_shader_program->setMatrix4("scale_matrix", scale_matrix);
        //p_shader_program->setMatrix4("rotate_matrix", rotate_matrix);
        //p_shader_program->setMatrix4("translate_matrix", translate_matrix);

        glm::mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
        p_shader_program->setMatrix4("model_matrix", model_matrix);

        p_vao->bind();
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(p_vao->get_indexes_count()), GL_UNSIGNED_INT, nullptr);

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
            //�������� ������-�������
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

                glViewport(0, 0, width, height);
            });

        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->isCompiled())
            return false;

        BufferLayout buffer_layout_1vec3 {

            ShaderDataType::Float3
        };

        BufferLayout buffer_layout_2vec3 {

            ShaderDataType::Float3,
            ShaderDataType::Float3
        };

        p_vao = std::make_unique<VertexArray>();
        p_positions_colors_vbo = std::make_unique<VertexBuffer>(positions_colors2, sizeof(positions_colors2), buffer_layout_2vec3);
        p_index_buffer = std::make_unique<IndexBuffer>(indexes, sizeof(indexes) / sizeof(GLuint));
        
        p_vao->add_vertex_buffer(*p_positions_colors_vbo);
        p_vao->set_index_buffer(*p_index_buffer);

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