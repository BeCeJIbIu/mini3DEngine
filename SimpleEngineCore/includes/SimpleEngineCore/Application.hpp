#pragma once
#include "SimpleEngineCore/Event.hpp"
#include <memory>

namespace SimpleEngine {

    class Application {
    public:
        Application();
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        virtual int start(unsigned int window_width, unsigned int window_height, const char* title);

        virtual void on_update();

        virtual ~Application();

    private:
        std::unique_ptr<class Window> m_pWindow; //!!!!!!!!! ����� Window ����������, ������� ����� class Window

        EventDispatcher m_event_dispatcher;
        bool m_bCloseWindow = false;
    };
}