#include "graphics/PostProcess.h"

#include "Common.h"
#include "Constants.h"
#include "Engine.h"
#include "Log.h"
#include "graphics/RenderTarget.h"
#include "io/FileSystem.h"

namespace mnd
{

namespace
{
constexpr const char *kOutlineVertPath = "shaders/outline.vert";
constexpr const char *kOutlineFragPath = "shaders/outline.frag";

GLuint CompileStage(GLenum stage, const char *src)
{
    GLuint s = glCreateShader(stage);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (ok == 0)
    {
        char log[kShaderInfoLogSize];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        LOG_ERROR("Outline shader compile failed: %s", log);
    }
    return s;
}

GLuint LinkProgram(GLuint vs, GLuint fs)
{
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (ok == 0)
    {
        char log[kShaderInfoLogSize];
        glGetProgramInfoLog(p, sizeof(log), nullptr, log);
        LOG_ERROR("Outline program link failed: %s", log);
        glDeleteProgram(p);
        return 0;
    }
    return p;
}
}  // namespace

bool PostProcess::Init()
{
    auto &fs   = Engine::GetInstance().GetFileSystem();
    auto vsSrc = fs.LoadAssetFileText(kOutlineVertPath);
    auto fsSrc = fs.LoadAssetFileText(kOutlineFragPath);
    if (vsSrc.empty() || fsSrc.empty())
    {
        LOG_ERROR("PostProcess: failed to load outline shader sources");
        return false;
    }

    GLuint vs  = CompileStage(GL_VERTEX_SHADER,   vsSrc.c_str());
    GLuint fs2 = CompileStage(GL_FRAGMENT_SHADER, fsSrc.c_str());
    m_program  = LinkProgram(vs, fs2);
    glDeleteShader(vs);
    glDeleteShader(fs2);
    if (m_program == 0)
    {
        return false;
    }

    glGenVertexArrays(1, &m_vao);

    m_locColor     = glGetUniformLocation(m_program, "uColor");
    m_locNormal    = glGetUniformLocation(m_program, "uNormal");
    m_locDepth     = glGetUniformLocation(m_program, "uDepth");
    m_locTexel     = glGetUniformLocation(m_program, "uTexel");
    m_locDepthStr  = glGetUniformLocation(m_program, "uDepthEdgeStrength");
    m_locNormalStr = glGetUniformLocation(m_program, "uNormalEdgeStrength");

    return true;
}

void PostProcess::Destroy()
{
    DestroyFBO();
    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

void PostProcess::DestroyFBO()
{
    if (m_color != 0) glDeleteTextures(1, &m_color);
    if (m_fbo   != 0) glDeleteFramebuffers(1, &m_fbo);
    m_color = m_fbo = 0;
    m_w = m_h = 0;
}

bool PostProcess::CreateFBO(int w, int h)
{
    DestroyFBO();
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_color);
    glBindTexture(GL_TEXTURE_2D, m_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("PostProcess FBO incomplete (0x%x) at %dx%d", status, w, h);
        DestroyFBO();
        return false;
    }
    m_w = w;
    m_h = h;
    return true;
}

void PostProcess::Resize(int w, int h)
{
    if (w == m_w && h == m_h && m_fbo != 0)
    {
        return;
    }
    CreateFBO(w, h);
}

void PostProcess::RunOutline(const RenderTarget &scene, const CameraData & /*cam*/)
{
    if (!IsValid())
    {
        return;
    }

    Resize(scene.Width(), scene.Height());

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_w, m_h);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glUseProgram(m_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene.ColorTex());
    if (m_locColor >= 0) glUniform1i(m_locColor, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, scene.NormalTex());
    if (m_locNormal >= 0) glUniform1i(m_locNormal, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, scene.DepthTex());
    if (m_locDepth >= 0) glUniform1i(m_locDepth, 2);

    if (m_locTexel     >= 0) glUniform2f(m_locTexel, 1.0F / static_cast<float>(m_w), 1.0F / static_cast<float>(m_h));
    if (m_locDepthStr  >= 0) glUniform1f(m_locDepthStr,  depthEdgeStrength);
    if (m_locNormalStr >= 0) glUniform1f(m_locNormalStr, normalEdgeStrength);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

}  // namespace mnd
