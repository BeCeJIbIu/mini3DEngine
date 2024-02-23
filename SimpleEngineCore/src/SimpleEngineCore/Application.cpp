#include "SimpleEngineCore/Application.hpp"
#include "SimpleEngineCore/Window.hpp"
#include <iostream>

namespace SimpleEngine {

    Application::Application() {

        std::cout << "Starting Application!\n";
    }

    int Application::start(unsigned int window_width, unsigned int window_height, const char* title) {

        m_pWindow = std::make_unique<Window>(title, window_width, window_height);
        m_pWindow->set_event_callback(
            [](Event& event) {
                std::cout << "[EVENT] Changed size to " << event.width << "x" << event.height << "\n";
            }
        );

        while (true) {
            m_pWindow->on_update();
            on_update();
        }
    }

    void Application::on_update() {

    }

    Application::~Application() {

        std::cout << "Closing Application!\n";
    }
}