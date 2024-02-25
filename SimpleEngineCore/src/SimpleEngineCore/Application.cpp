#include "SimpleEngineCore/Application.hpp"
#include "SimpleEngineCore/Window.hpp"
#include "SimpleEngineCore/Event.hpp"
#include <iostream>

namespace SimpleEngine {

    Application::Application() {

        std::cout << "Starting Application!\n";
    }

    int Application::start(unsigned int window_width, unsigned int window_height, const char* title) {

        m_pWindow = std::make_unique<Window>(title, window_width, window_height);

        m_event_dispatcher.add_event_listener<EventMouseMoved>(
            [](EventMouseMoved& event) {
                std::cout << "[EVENT] Mouse Moved to " << event.x << "x" << event.y << "\n";
            }
        );

        m_event_dispatcher.add_event_listener<EventWindowResize>(
            [](EventWindowResize& event) {
                std::cout << "[EVENT] Changed size to " << event.width << "x" << event.height << "\n";
            }
        );

        m_event_dispatcher.add_event_listener<EventWindowClose>(
            [&](EventWindowClose& event) {
                std::cout << "[EVENT] Window Close\n";
                m_bCloseWindow = true;
            }
        );

        m_pWindow->set_event_callback(
            [&](BaseEvent& event) {
                m_event_dispatcher.dispatch(event);
            }
        );

        while (!m_bCloseWindow) {
            m_pWindow->on_update();
            on_update();
        }
        m_pWindow = nullptr;

        return 0;
    }

    void Application::on_update() {
        //
    }

    Application::~Application() {

        std::cout << "Closing Application!\n";
    }
}