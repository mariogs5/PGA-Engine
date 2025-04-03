//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "BufferManagement.h"

#include <glad/glad.h>
#include <stdexcept>

typedef glm::vec2  vec2;
typedef glm::vec3  vec3;
typedef glm::vec4  vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};

struct Texture
{
    GLuint      handle;
    std::string filepath;
};

struct VertexShaderAttribute {
    u8 location;
    u8 componentCount;
};

struct VertexShaderLayout {
    std::vector<VertexShaderAttribute> attributes;
};

struct Program
{
    GLuint             handle;
    std::string        filepath;
    std::string        programName;
    u64                lastWriteTimestamp; // What is this for?
    VertexShaderLayout vertexInputLayout;
};

enum Mode
{
    Mode_TexturedQuad,
    Mode_Forward_Geometry,
    Mode_Count
};

// Textured Quad
struct VertexV3U2
{
    glm::vec3 pos;
    glm::vec2 uv;
};

struct Entity 
{
    glm::mat4 worldMatrix;
    u32 modelIndex;
    u32 entityBufferOffset;
    u32 entityBufferSize;
};

enum LightType
{
    LightType_Directional, 
    LightType_Point  
};

struct Light
{
    std::string name;
    LightType type; 
    vec3 color;        
    vec3 direction;    
    vec3 position;   
};

struct FrameBuffer 
{
    GLuint handle;
    std::vector<std::pair<GLenum,GLuint>> attachments;
    GLuint depthHandle;

    bool CreateFBO(const uint64_t aAtachments, const uint64_t displayWidth, const uint64_t displayHeight)
    {
        if (aAtachments > GL_MAX_COLOR_ATTACHMENTS) 
        {
            return false;
        }

        std::vector<GLenum> enums;
        for (size_t i = 0; i < aAtachments; ++i)
        {
            GLuint colorAttachment;
            glGenTextures(1, &colorAttachment);
            glBindTexture(GL_TEXTURE_2D, colorAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayWidth, displayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);

            attachments.push_back({ GL_COLOR_ATTACHMENT0 + i, colorAttachment });
            enums.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        glGenTextures(1, &depthHandle);
        glBindTexture(GL_TEXTURE_2D, depthHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, displayWidth, displayHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenFramebuffers(1, &handle);
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
        for (auto it = attachments.begin(); it != attachments.end(); ++it)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, it->first, GL_TEXTURE_2D, it->second, 0);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthHandle, 0);

        GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            const char* error = "";
            switch (framebufferStatus) {
            case GL_FRAMEBUFFER_UNDEFINED: error = "UNDEFINED"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: error = "INCOMPLETE_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: error = "MISSING_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: error = "INCOMPLETE_DRAW_BUFFER"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: error = "INCOMPLETE_READ_BUFFER"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: error = "UNSUPPORTED"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: error = "INCOMPLETE_MULTISAMPLE"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: error = "INCOMPLETE_LAYER_TARGETS"; break;
            }
            throw std::runtime_error(std::string("Framebuffer creation error: ") + error);
        }

        glDrawBuffers(enums.size(), enums.data());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Clean()
    {
        glDeleteFramebuffers(1, &handle);
        for (auto& texture : attachments) 
        {
            glDeleteTextures(1, &texture.second);
            texture.second = 0;
        }
        glDeleteTextures(1, &depthHandle);
        depthHandle = 0;
    }
};

const VertexV3U2 vertices[] = {
    { glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0) }, // bottom-left vertex
    { glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0) }, // bottom-right vertex
    { glm::vec3(1.0,  1.0, 0.0), glm::vec2(1.0, 1.0) }, // top-right vertex
    { glm::vec3(-1.0,  1.0, 0.0), glm::vec2(0.0, 1.0) }  // top-left vertex
};

const u16 indices[] = {
    0, 1, 2,
    0, 2, 3
};

struct OpenGLInfo {
    std::string version;
    std::string renderer;
    std::string vendor;
    std::string glslVersion;
    std::vector<std::string> extensions;
};

struct App
{
    // Loop
    f32  deltaTime;
    bool isRunning;

    // Input
    Input input;

    // Graphics
    char gpuName[64];
    char openGlVersion[64];
    std::string mOpenGLInfo;

    ivec2 displaySize;
    
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    std::vector<Model> models;

    std::vector<Texture>  textures;
    std::vector<Program>  programs;

    // program index
    u32 texturedGeometryProgramIdx;

    // Patrick
    u32 texturedMeshProgramIdx; // Program Index
    u32 patrickIdx;             // Model Index
    u32 patrickTextureUniform;  // Texture Index

    
    // texture indices
    u32 diceTexIdx;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;

    // Mode
    Mode mode;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint vao;

    GLint maxUniformBufferSize;
    GLint uniformBlockAlignment;
    GLuint bufferHandle;

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint embeddedVertices;
    GLuint embeddedElements;

    // Location of the texture uniform in the textured quad shader
    GLuint programUniformTexture;

    // --- Camera --- //
    Camera camera;

    // --- OpenGl Info --- //
    OpenGLInfo glInfo;

    // --- Uniform Buffers --- //
    Buffer globalUBO;
    Buffer entityUBO;

    std::vector<Entity> entities;
    std::vector<Light> lights;

    FrameBuffer primaryFBO;

    std::vector<std::string> GBufferItems;
    int currentGBufferItem;
};

void UpdateLights(App* app);

void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

void Cleanup(App* app);

GLuint FindVAO(Mesh& mesh, u32 submeshIndex, const Program& program);

void CreateVAO(Mesh& mesh, Submesh& submesh, const Program& program, GLuint& vaoHandle);

void CreateFBO(App* app);

OpenGLInfo GetOpenGLInfo(OpenGLInfo& glInfo);

