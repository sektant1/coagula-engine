#include "Engine.h"
#include "Application.h"
#include "scene/GameObject.h"
#include "scene/Component.h"
#include "scene/components/CameraComponent.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace eng
{
    void keyCallback(GLFWwindow* window, int key, int, int action, int)
    {
        auto& inputManager = eng::Engine::GetInstance().GetInputManager();
        if (action == GLFW_PRESS)
        {
            inputManager.SetKeyPressed(key, true);
        }
        else if (action == GLFW_RELEASE)
        {
            inputManager.SetKeyPressed(key, false);
        }
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int)
    {
        auto& inputManager = eng::Engine::GetInstance().GetInputManager();
        if (action == GLFW_PRESS)
        {
            inputManager.SetMouseButtonPressed(button, true);
        }
        else if (action == GLFW_RELEASE)
        {
            inputManager.SetMouseButtonPressed(button, false);
        }
    }

    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
    {
        auto& inputManager = eng::Engine::GetInstance().GetInputManager();

        inputManager.SetMousePositionOld(inputManager.GetMousePositionCurrent());

        glm::vec2 currentPos(static_cast<float>(xpos), static_cast<float>(ypos));
        inputManager.SetMousePositionCurrent(currentPos);
    }

    Engine& Engine::GetInstance()
    {
        static Engine instance;
        return instance;
    }

    bool Engine::Init(int width, int height)
    {
        if (!m_application)
        {
            return false;
        }

#if defined (__linux__)
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

        if (!glfwInit())
        {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(width, height, "GameDevelopmentProject", nullptr, nullptr);

        if (m_window == nullptr)
        {
            std::cout << "Error creating window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPositionCallback);

        glfwMakeContextCurrent(m_window);

        if (glewInit() != GLEW_OK)
        {
            glfwTerminate();
            return false;
        }

        m_graphicsAPI.Init();
        return m_application->Init();
    }

    void Engine::Run()
    {
        if (!m_application)
        {
            return;
        }

        m_lastTimePoint = std::chrono::steady_clock::now();

        while (!glfwWindowShouldClose(m_window) && !m_application->NeedsToBeClosed())
        {
            glfwPollEvents();

            auto now = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(now - m_lastTimePoint).count();
            m_lastTimePoint = now;

            m_application->Update(deltaTime);

            m_graphicsAPI.SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            m_graphicsAPI.ClearBuffers();

            CameraData cameraData;

            int width = 0;
            int height = 0;
            glfwGetWindowSize(m_window, &width, &height);
            float aspect = static_cast<float>(width) / static_cast<float>(height);

            if (m_currentScene)
            {
                if (auto cameraObject = m_currentScene->GetMainCamera())
                {
                    // logic for matrices
                    auto cameraComponent = cameraObject->GetComponent<CameraComponent>();
                    if (cameraComponent)
                    {
                        cameraData.viewMatrix = cameraComponent->GetViewMatrix();
                        cameraData.projectionMatrix = cameraComponent->GetProjectionMatrix(aspect);
                    }
                }
            }

            m_rederQueue.Draw(m_graphicsAPI, cameraData);

            glfwSwapBuffers(m_window);

            m_inputManager.SetMousePositionOld(m_inputManager.GetMousePositionCurrent());
        }
    }

    void Engine::Destroy()
    {
        if (m_application)
        {
            m_application->Destroy();
            m_application.reset();
            glfwTerminate();
            m_window = nullptr;
        }
    }

    void Engine::SetApplication(Application* app)
    {
        m_application.reset(app);
    }

    Application* Engine::GetApplication()
    {
        return m_application.get();
    }

    InputManager& Engine::GetInputManager()
    {
        return m_inputManager;
    }

    GraphicsAPI& Engine::GetGraphicsAPI()
    {
        return m_graphicsAPI;
    }

    RenderQueue& Engine::GetRenderQueue()
    {
        return m_rederQueue;
    }

    FileSystem& Engine::GetFileSystem()
    {
        return m_fileSystem;
    }

    void Engine::SetScene(Scene* scene)
    {
        m_currentScene.reset(scene);
    }

    Scene* Engine::GetScene()
    {
        return m_currentScene.get();
    }
}