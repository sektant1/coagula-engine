#include "render/ParticleSystem.h"

#include <algorithm>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Common.h"
#include "Engine.h"
#include "Log.h"
#include "graphics/GraphicsAPI.h"
#include "graphics/ShaderProgram.h"
#include "render/Builder.h"
#include "render/Mesh.h"

namespace mnd
{

namespace
{
constexpr const char *kParticleVertPath = "shaders/particle.vert";
constexpr const char *kParticleFragPath = "shaders/particle.frag";
}  // namespace

void ParticleSystem::Init()
{
    auto data = Builder::CreateRectangle(1.0F, 1.0F);
    m_quad    = data.buildMesh();

    auto &fs   = Engine::GetInstance().GetFileSystem();
    auto  vert = fs.LoadAssetFileText(kParticleVertPath);
    auto  frag = fs.LoadAssetFileText(kParticleFragPath);
    if (vert.empty() || frag.empty())
    {
        LOG_ERROR("ParticleSystem::Init failed to load shader sources");
        return;
    }

    m_shader = Engine::GetInstance().GetGraphicsAPI().CreateShaderProgram(vert, frag);
    if (!m_shader)
    {
        LOG_ERROR("ParticleSystem::Init failed to compile particle shader");
    }

    m_particles.reserve(m_capacity);
}

void ParticleSystem::Shutdown()
{
    m_particles.clear();
    m_shader.reset();
    m_quad.reset();
}

void ParticleSystem::Update(f32 deltaTime)
{
    if (m_paused || deltaTime <= 0.0F)
    {
        return;
    }

    for (auto &p : m_particles)
    {
        p.velocity += p.acceleration * deltaTime;
        p.position += p.velocity * deltaTime;
        p.age += deltaTime;
    }

    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(), [](const Particle &p) { return !p.IsAlive(); }),
        m_particles.end()
    );
}

void ParticleSystem::Render(const CameraData &cameraData)
{
    if (!m_shader || !m_quad || m_particles.empty())
    {
        return;
    }

    // Camera-aligned billboard basis: extract right and up from the view matrix.
    const glm::mat4 &view  = cameraData.viewMatrix;
    const vec3       right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    const vec3       up    = glm::vec3(view[0][1], view[1][1], view[2][1]);
    const vec3       fwd   = glm::cross(right, up);

    GLboolean prevDepthMask  = GL_TRUE;
    GLboolean prevBlend      = GL_FALSE;
    GLint     prevSrcRGB     = 0;
    GLint     prevDstRGB     = 0;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
    prevBlend = glIsEnabled(GL_BLEND);
    glGetIntegerv(GL_BLEND_SRC_RGB, &prevSrcRGB);
    glGetIntegerv(GL_BLEND_DST_RGB, &prevDstRGB);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // additive

    m_shader->Bind();
    m_shader->SetUniform("uView", cameraData.viewMatrix);
    m_shader->SetUniform("uProjection", cameraData.projectionMatrix);

    auto &graphicsAPI = Engine::GetInstance().GetGraphicsAPI();
    graphicsAPI.BindMesh(m_quad.get());

    for (const auto &p : m_particles)
    {
        const float t        = std::clamp(p.age / std::max(p.lifetime, 1e-4F), 0.0F, 1.0F);
        const float size     = glm::mix(p.sizeStart, p.sizeEnd, t);
        const vec4  color    = glm::mix(p.colorStart, p.colorEnd, t);

        glm::mat4 model(0.0F);
        model[0] = glm::vec4(right * size, 0.0F);
        model[1] = glm::vec4(up * size, 0.0F);
        model[2] = glm::vec4(fwd * size, 0.0F);
        model[3] = glm::vec4(p.position, 1.0F);

        m_shader->SetUniform("uModel", model);
        m_shader->SetUniform("uColor", color.r, color.g, color.b, color.a);
        graphicsAPI.DrawMesh(m_quad.get());
    }

    graphicsAPI.UnbindMesh(m_quad.get());

    // Restore prior GL state.
    glBlendFunc(static_cast<GLenum>(prevSrcRGB), static_cast<GLenum>(prevDstRGB));
    if (prevBlend == GL_FALSE)
    {
        glDisable(GL_BLEND);
    }
    glDepthMask(prevDepthMask);
}

void ParticleSystem::Spawn(const Particle &particle)
{
    if (m_particles.size() >= m_capacity)
    {
        // Drop oldest to keep recent emissions visible. O(n) but capacity is small.
        m_particles.erase(m_particles.begin());
    }
    m_particles.push_back(particle);
}

}  // namespace mnd
