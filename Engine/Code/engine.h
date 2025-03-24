//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "BufferManagement.h"

#include <glad/glad.h>

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
    LightType type; 
    vec3 color;        
    vec3 direction;    
    vec3 position;   
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
};

void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

void Cleanup(App* app);

GLuint FindVAO(Mesh& mesh, u32 submeshIndex, const Program& program);

void CreateVAO(Mesh& mesh, Submesh& submesh, const Program& program, GLuint& vaoHandle);

OpenGLInfo GetOpenGLInfo(OpenGLInfo& glInfo);

