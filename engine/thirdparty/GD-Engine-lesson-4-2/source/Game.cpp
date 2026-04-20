#include "Game.h"
#include "TestObject.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


bool Game::Init()
{
    auto& fs = eng::Engine::GetInstance().GetFileSystem();
    auto path = fs.GetAssetsFolder() / "brick.png";

    int width, height, channels;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    std::shared_ptr<eng::Texture> texture;
    if (data)
    {
        texture = std::make_shared<eng::Texture>(width, height, channels, data);
        stbi_image_free(data);
    }

    m_scene = new eng::Scene();

    auto camera = m_scene->CreateObject("Camera");
    camera->AddComponent(new eng::CameraComponent());
    camera->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));
    camera->AddComponent(new eng::PlayerControllerComponent());

    m_scene->SetMainCamera(camera);

    m_scene->CreateObject<TestObject>("TestObject");


    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;
        layout (location = 2) in vec2 uv;

        out vec3 vColor;
        out vec2 vUV;

        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;

        void main()
        {
            vColor = color;
            vUV = uv;
            gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 vColor;
        in vec2 vUV;

        uniform sampler2D brickTexture;

        void main()
        {
            vec4 texColor = texture(brickTexture, vUV);
            FragColor = texColor * vec4(vColor, 1.0);
        }
    )";

    auto& graphicsAPI = eng::Engine::GetInstance().GetGraphicsAPI();
    auto shaderProgram = graphicsAPI.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);


    
    auto material = std::make_shared<eng::Material>();
    material->SetShaderProgram(shaderProgram);
    material->SetParam("brickTexture", texture);

    std::vector<float> vertices =
    {
        // Front face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // Top face 
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // Right face
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // Left face
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // Bottom face
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // Back face
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
    };

    std::vector<unsigned int> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,
        // top face
        4, 5, 6,
        4, 6, 7,
        // right face
        8, 9, 10,
        8, 10, 11,
        // left face
        12, 13, 14,
        12, 14, 15,
        // bottom face
        16, 17, 18,
        16, 18, 19,
        // back face
        20, 21, 22,
        20, 22, 23
    };

    eng::VertexLayout vertexLayout;

    // Postion
    vertexLayout.elements.push_back({
        0,
        3,
        GL_FLOAT,
        0
        });
    // Color
    vertexLayout.elements.push_back({
        1,
        3,
        GL_FLOAT,
        sizeof(float) * 3
        });
    // UV
    vertexLayout.elements.push_back({
        2,
        2,
        GL_FLOAT,
        sizeof(float) * 6
        });
    vertexLayout.stride = sizeof(float) * 8;

    auto mesh = std::make_shared<eng::Mesh>(vertexLayout, vertices, indices);

    auto objectA = m_scene->CreateObject("ObjectA");
    objectA->AddComponent(new eng::MeshComponent(material, mesh));
    objectA->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));

    auto objectB = m_scene->CreateObject("ObjectB");
    objectB->AddComponent(new eng::MeshComponent(material, mesh));
    objectB->SetPosition(glm::vec3(0.0f, 2.0f, 2.0f));
    objectB->SetRotation(glm::vec3(0.0f, 2.0f, 0.0f));

    auto objectC = m_scene->CreateObject("ObjectC");
    objectC->AddComponent(new eng::MeshComponent(material, mesh));
    objectC->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    objectC->SetRotation(glm::vec3(1.0f, 0.0f, 1.0f));
    objectC->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

    
    eng::Engine::GetInstance().SetScene(m_scene);
    
    return true;
}

void Game::Update(float deltaTime)
{
    m_scene->Update(deltaTime);
}

void Game::Destroy()
{

}