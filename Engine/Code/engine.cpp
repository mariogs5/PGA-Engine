//
// engine.cpp : Put all your graphics stuff in this file. This is kind of the graphics module.
// In here, you should type all your OpenGL commands, and you can also type code to handle
// input platform events (e.g to move the camera or react to certain shortcuts), writing some
// graphics related GUI options, and so on.
//

#include "engine.h"
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include "OpenGLErrorGuard.h"
#include <format>
#include <imgui_impl_opengl3.h>

void CreateEntity(App* app, u32 modelIndex, u32 textureIndex, glm::vec3 pos, glm::mat4 VP)
{
    AlignHead(app->entityUBO, app->uniformBlockAlignment);
    Entity entity;
    entity.entityBufferOffset = app->entityUBO.head;
    entity.worldMatrix = glm::translate(pos);
    entity.modelIndex = modelIndex;
    entity.textureIndex = textureIndex;
    PushMat4(app->entityUBO, entity.worldMatrix);
    PushMat4(app->entityUBO, VP * entity.worldMatrix);
    entity.entityBufferSize = app->entityUBO.head - entity.entityBufferOffset;
    app->entities.push_back(entity);
}

void CreateEntities(App* app) 
{
    // --- Default textures --- //
    app->diceTexIdx = LoadTexture2D(app, "dice.png");
    app->whiteTexIdx = LoadTexture2D(app, "color_white.png");
    app->blackTexIdx = LoadTexture2D(app, "color_black.png");
    app->normalTexIdx = LoadTexture2D(app, "color_normal.png");
    app->magentaTexIdx = LoadTexture2D(app, "color_magenta.png");
    app->greenTexIdx = LoadTexture2D(app, "color_green.png");
    app->skyBlueTexIdx = LoadTexture2D(app, "color_skyblue.png");

    Program& ModelProgram = app->programs[app->texturedMeshProgramIdx];

    //app->UpdateLights(app);

    app->textureUniform = glGetUniformLocation(ModelProgram.handle, "uTexture");

      // --- Entities UBO --- //
    app->entityUBO = CreateConstantBuffer(app->maxUniformBufferSize);
    MapBuffer(app->entityUBO, GL_WRITE_ONLY);
    glm::mat4 VP = app->camera.GetProjectionMatrix() * app->camera.GetViewMatrix();

    // --- Floor Model --- //
    app->floorIdx = LoadModel(app, "Models/Plane.obj");
    CreateEntity(app, app->floorIdx, app->whiteTexIdx, glm::vec3(0, 0, 0), VP);

    // --- Cube Model --- //
    app->cubeIdx = LoadModel(app, "Models/Cube.obj");
    CreateEntity(app, app->cubeIdx, app->normalTexIdx, glm::vec3(-7, 1, 0), VP);

    // --- Sphere Model --- //
    app->sphereIdx = LoadModel(app, "Models/Sphere.obj");
    CreateEntity(app, app->sphereIdx, app->greenTexIdx, glm::vec3(-3, 0, 7), VP);

    // --- Cone Model --- //
    app->coneIdx = LoadModel(app, "Models/Cone.obj");
    CreateEntity(app, app->coneIdx, app->magentaTexIdx, glm::vec3(7, 0, 0), VP);

    // --- Torus Model --- //
    app->torusIdx = LoadModel(app, "Models/Torus.obj");
    CreateEntity(app, app->torusIdx, app->skyBlueTexIdx, glm::vec3(0, 0, -10), VP);

    // --- Patrick Model --- //
    app->patrickIdx = LoadModel(app, "Patrick/Patrick.obj");

    AlignHead(app->entityUBO, app->uniformBlockAlignment);
    Entity patrick;
    patrick.entityBufferOffset = app->entityUBO.head;
    patrick.worldMatrix = glm::translate(glm::vec3(0, 0, 0));
    patrick.modelIndex = app->patrickIdx;
    PushMat4(app->entityUBO, patrick.worldMatrix);
    PushMat4(app->entityUBO, VP * patrick.worldMatrix);
    patrick.entityBufferSize = app->entityUBO.head - patrick.entityBufferOffset;
    app->entities.push_back(patrick);


    // --- Repo Model --- //
    glm::vec3 repoPos = glm::vec3(3, 0, 7);
    app->repoBodyIdx = LoadModel(app, "Models/RepoBody.obj");
    CreateEntity(app, app->repoBodyIdx, app->magentaTexIdx, repoPos, VP);

    app->repoEyesIdx = LoadModel(app, "Models/RepoEyes.obj");
    CreateEntity(app, app->repoEyesIdx, app->whiteTexIdx, repoPos, VP);

    app->repoPupilsIdx = LoadModel(app, "Models/RepoPupils.obj");
    CreateEntity(app, app->repoPupilsIdx, app->blackTexIdx, repoPos, VP);

    UnmapBuffer(app->entityUBO);
}

void CreateNewLight(App* app, std::string name, LightType type, vec3 color, vec3 direction, vec3 position)
{
    Light light = { name, type, color, direction, position };
    app->lights.push_back(light);
}

void CreateDefaultLights(App* app)
{
    app->lights.clear();

    Light sun = {
        "Sun",
        LightType_Directional,
        vec3(0.9f, 0.85f, 0.7f),  
        vec3(-0.5f, -1.0f, 0.2f),
        vec3(0.0f)
    };
    app->lights.push_back(sun);

    // Accent point light (right)
    Light pointRight = {
        "WarmAccent",
        LightType_Point,
        vec3(0.8f, 0.6f, 0.4f), 
        vec3(0.0f),
        vec3(2.0f, 1.5f, 2.0f)
    };
    app->lights.push_back(pointRight);

    // Accent point light (left)
    Light pointLeft = {
        "CoolAccent",
        LightType_Point,
        vec3(0.4f, 0.6f, 0.8f),     
        vec3(0.0f),
        vec3(-2.0f, 1.5f, -1.0f)
    };
    app->lights.push_back(pointLeft);

    // Ambient fill light
    Light fill = {
        "AmbientFill",
        LightType_Point,
        vec3(0.3f, 0.4f, 0.5f), 
        vec3(0.0f),
        vec3(0.0f, 3.0f, 0.0f) 
    };
    app->lights.push_back(fill);
}

void CreateLightStressTest(App* app)
{
    app->lights.clear();

    Light sun = {
       "Sun",
       LightType_Directional,
       vec3(0.9f, 0.85f, 0.7f),
       vec3(-0.5f, -1.0f, 0.2f),
       vec3(0.0f)
    };
    app->lights.push_back(sun);
    
    int gridSize = 31;
    float totalSpan = 30.0f;          
    float spacing = totalSpan / (gridSize - 1);
    float halfSpan = totalSpan / 2.0f;

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            // Calculate positions centered around (0, 3, 0)
            float x = -halfSpan + i * spacing;
            float z = -halfSpan + j * spacing;

            Light light = {
                std::format("Stress Light {}", i * gridSize + j),
                LightType_Point,
                vec3(0.3f, 0.4f, 0.5f),    // Color
                vec3(0.0f),                // Direction (unused for point lights)
                vec3(x, 3.0f, z)           // Position
            };
            app->lights.push_back(light);
        }
    }
    
    //app->UpdateLights(app);
}

GLuint CreateProgramFromSource(String programSource, const char* shaderName)
{
    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 430\n";
    char shaderNameDefine[128];
    sprintf(shaderNameDefine, "#define %s\n", shaderName);
    char vertexShaderDefine[] = "#define VERTEX\n";
    char fragmentShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        shaderNameDefine,
        vertexShaderDefine,
        programSource.str
    };
    const GLint vertexShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(vertexShaderDefine),
        (GLint) programSource.len
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        shaderNameDefine,
        fragmentShaderDefine,
        programSource.str
    };
    const GLint fragmentShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(fragmentShaderDefine),
        (GLint) programSource.len
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vshader);
    glAttachShader(programHandle, fshader);
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    glUseProgram(0);

    glDetachShader(programHandle, vshader);
    glDetachShader(programHandle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programHandle;
}

u32 LoadProgram(App* app, const char* filepath, const char* programName)
{
    String programSource = ReadTextFile(filepath);

    Program program = {};
    program.handle = CreateProgramFromSource(programSource, programName);
    program.filepath = filepath;
    program.programName = programName;
    program.lastWriteTimestamp = GetFileLastWriteTimestamp(filepath);

    if (program.handle != 0) 
    {
        GLint AttributeCount = 0UL;
        glGetProgramiv(program.handle, GL_ACTIVE_ATTRIBUTES, &AttributeCount);

        for (size_t i = 0; i < AttributeCount; ++i) 
        {
            char Name[248];
            GLsizei realNameSize = 0UL;
            GLsizei attribSize = 0UL;
            GLenum attribType;

            glGetActiveAttrib(program.handle, i, ARRAY_COUNT(Name), &realNameSize, &attribSize, &attribType, Name);
            GLuint attribLocation = glGetAttribLocation(program.handle, Name);
            program.vertexInputLayout.attributes.push_back({ static_cast <u8>(attribLocation),static_cast <u8>(attribSize) });
        }
    }

    app->programs.push_back(program);

    return app->programs.size() - 1;
}

Image LoadImage(const char* filename)
{
    Image img = {};
    stbi_set_flip_vertically_on_load(true);
    img.pixels = stbi_load(filename, &img.size.x, &img.size.y, &img.nchannels, 0);
    if (img.pixels)
    {
        img.stride = img.size.x * img.nchannels;
    }
    else
    {
        ELOG("Could not open file %s", filename);
    }
    return img;
}

void FreeImage(Image image)
{
    stbi_image_free(image.pixels);
}

GLuint CreateTexture2DFromImage(Image image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat     = GL_RGB;
    GLenum dataType       = GL_UNSIGNED_BYTE;

    switch (image.nchannels)
    {
        case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
        case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
        default: ELOG("LoadTexture2D() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texHandle;
}

u32 LoadTexture2D(App* app, const char* filepath)
{
    for (u32 texIdx = 0; texIdx < app->textures.size(); ++texIdx)
        if (app->textures[texIdx].filepath == filepath)
            return texIdx;

    Image image = LoadImage(filepath);

    if (image.pixels)
    {
        Texture tex = {};
        tex.handle = CreateTexture2DFromImage(image);
        tex.filepath = filepath;

        u32 texIdx = app->textures.size();
        app->textures.push_back(tex);

        FreeImage(image);
        return texIdx;
    }
    else
    {
        return UINT32_MAX;
    }
}

void UpdateGBuffer(App* app)
{
    Program& programTexturedGeometry = app->programs[app->texturedGeometryProgramIdx];
    glUseProgram(programTexturedGeometry.handle);

    if (app->currentGBufferItem == 3)
    {
        GLfloat nearPlaneProgramLoc = glGetUniformLocation(programTexturedGeometry.handle, "uNear");
        glUniform1f(nearPlaneProgramLoc, app->camera.GetNearPlane());

        GLfloat farPlaneProgramLoc = glGetUniformLocation(programTexturedGeometry.handle, "uFar");
        glUniform1f(farPlaneProgramLoc, app->camera.GetFarPlane());
    }

    GLint gBufferProgramLoc = glGetUniformLocation(programTexturedGeometry.handle, "uGBuffer");
    glUniform1i(gBufferProgramLoc, app->currentGBufferItem);

    glUseProgram(0);
}

void App::UpdateLights(App* app)
{
    MapBuffer(app->globalUBO, GL_WRITE_ONLY);
    PushVec3(app->globalUBO, app->camera.GetPosition());

    PushUInt(app->globalUBO, app->lights.size());
    for (u32 i = 0; i < app->lights.size(); ++i)
    {
        AlignHead(app->globalUBO, sizeof(vec4));

        Light& light = app->lights[i];
        PushUInt(app->globalUBO, light.type);
        PushVec3(app->globalUBO, light.color);
        PushVec3(app->globalUBO, light.direction);
        PushVec3(app->globalUBO, light.position);
    }
    UnmapBuffer(app->globalUBO);
}

void App::UpdateCameraUniforms(App* app)
{
    UpdateLights(app);

    MapBuffer(app->entityUBO, GL_WRITE_ONLY);
    glm::mat4 VP = app->camera.GetProjectionMatrix() * app->camera.GetViewMatrix();

    for (int i = 0; i < app->entities.size(); ++i) 
    {
        Entity& entity = app->entities[i];

        glm::mat4 mvp = VP * entity.worldMatrix;

        u32 mvpOffset = entity.entityBufferOffset + sizeof(glm::mat4);
        WriteData(app->entityUBO, mvpOffset, mvp);
    }
    UnmapBuffer(app->entityUBO);
}

void App::OnResizeWindow(int width, int height)
{
    displaySize = vec2 (width, height);
    //Hacer esto para arreglar aspect ratio
    camera.SetAspectRatio(this, static_cast<float>(displaySize.x) / static_cast<float>(displaySize.y));
    primaryFBO.Clean();
    primaryFBO.CreateFBO(4, displaySize.x, displaySize.y);
}

void RenderScreenFillQuad(App* app, const FrameBuffer& aFBO)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, app->displaySize.x, app->displaySize.y);

    Program& programTexturedGeometry = app->programs[app->texturedGeometryProgramIdx];
    glUseProgram(programTexturedGeometry.handle);

    glBindVertexArray(app->vao);

    int iteration = 0;
    const char* uniformNames[] = { "uColor", "uNormals", "uPosition", "uViewDir" };

    for(const auto& texture: aFBO.attachments)
    {
        GLuint uniformPosition = glGetUniformLocation(programTexturedGeometry.handle, uniformNames[iteration]);

        glActiveTexture(GL_TEXTURE0 + iteration);
        glBindTexture(GL_TEXTURE_2D, texture.second);
        glUniform1i(uniformPosition, iteration);

        ++iteration;
    }
    GLuint uniformPosition = glGetUniformLocation(programTexturedGeometry.handle, "uDepth");
    glActiveTexture(GL_TEXTURE0 + iteration);
    glBindTexture(GL_TEXTURE_2D, aFBO.depthHandle);
    glUniform1i(uniformPosition, iteration);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Init(App* app)
{
    app->glInfo = GetOpenGLInfo(app->glInfo);

    app->GBufferItems = { "Final Render", "Albedo", "Normals", "Depth", "Position", "View Direction" };
    app->currentGBufferItem = 0;

    glEnable(GL_DEPTH_TEST);

    // VBO (Vertex)
    glGenBuffers(1, &app->embeddedVertices);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // EBO (indices)
    glGenBuffers(1, &app->embeddedElements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // VAO
    glGenVertexArrays(1, &app->vao);
    glBindVertexArray(app->vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3U2), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3U2), (void*)12);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBindVertexArray(0);

    // --------- Programs --------- //
    app->texturedMeshProgramIdx = LoadProgram(app, "shaders/RENDER_GEOMETRY.glsl", "RENDER_GEOMETRY");   // Render Geometry
    app->texturedGeometryProgramIdx = LoadProgram(app, "shaders/RENDER_QUAD.glsl", "TEXTURED_GEOMETRY"); // Textured Quad

    // --- Create Uniforms --- //
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &app->maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->uniformBlockAlignment);

    // --- Lights --- //
    CreateDefaultLights(app);

    // --- Global (Lights) UBO --- //
    app->globalUBO = CreateConstantBuffer(app->maxUniformBufferSize);
    app->UpdateLights(app);

    // --- Entities UBO --- //
    CreateEntities(app);

    app->mode = Mode_Forward_Geometry;

    //app->primaryFBO.CreateFBO(4, app->displaySize.x, app->displaySize.y);
    app->OnResizeWindow(app->displaySize.x, app->displaySize.y);
}

void Gui(App* app)
{
    ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);

    InfoWindow(app);
}

void Update(App* app)
{
    app->camera.Update(app);
}

void Render(App* app)
{
    switch (app->mode)
    {
        case Mode_TexturedQuad:
        {
            glClearColor(0.f, 0.f, 0.f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, app->displaySize.x, app->displaySize.y);

            Program& programTexturedGeometry = app->programs[app->texturedGeometryProgramIdx];
            glUseProgram(programTexturedGeometry.handle);
            glBindVertexArray(app->vao);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glUniform1i(app->programUniformTexture, 0);
            glActiveTexture(GL_TEXTURE0);
            GLuint textureHandle = app->textures[app->diceTexIdx].handle;
            glBindTexture(GL_TEXTURE_2D, textureHandle);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
        break;
        case Mode_Forward_Geometry:
        {
            glClearColor(0.f, 0.f, 0.f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindFramebuffer(GL_FRAMEBUFFER, app->primaryFBO.handle);

            std::vector<GLuint>textures;
            for(auto& it:app->primaryFBO.attachments)
            {
                textures.push_back(it.second);
            }
            glDrawBuffers(textures.size(), textures.data());

            glClearColor(0.f, 0.f, 0.f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, app->displaySize.x, app->displaySize.y);

            Program& textureMeshProgram = app->programs[app->texturedMeshProgramIdx];
            glUseProgram(textureMeshProgram.handle);

            glBindBufferRange(GL_UNIFORM_BUFFER, 0, app->globalUBO.handle, 0, app->globalUBO.size);

            for (const auto& entity : app->entities) 
            {
                glBindBufferRange(GL_UNIFORM_BUFFER, 1, app->entityUBO.handle, entity.entityBufferOffset, entity.entityBufferSize);

                Model& model = app->models[entity.modelIndex];
                Mesh& mesh = app->meshes[model.meshIdx];

                for (u32 i = 0; i < mesh.submeshes.size(); ++i) {
                    GLuint vao = FindVAO(mesh, i, textureMeshProgram);
                    glBindVertexArray(vao);

                    u32 submeshMaterialIdx = model.materialIdx[i];
                    Material& submeshMaterial = app->materials[submeshMaterialIdx];

                    if (submeshMaterial.albedoTextureIdx > 0) 
                    {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, app->textures[submeshMaterial.albedoTextureIdx].handle);
                        glUniform1i(app->textureUniform, 0);
                    }
                    else 
                    {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, app->textures[entity.textureIndex].handle);
                        glUniform1i(app->textureUniform, 0);
                    }

                    Submesh& submesh = mesh.submeshes[i];
                    glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(u64)submesh.indexOffset);

                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }

            glUseProgram(0);
            RenderScreenFillQuad(app, app->primaryFBO);
            glBindBuffer(GL_FRAMEBUFFER, 0);
        }
        break;

        default:;
    }
}

void Cleanup(App* app)
{
    for (auto& texture : app->textures) 
    {
        glDeleteTextures(1, &texture.handle);
        texture.handle = 0;
    }

    for (auto& program : app->programs)
    {
        glDeleteProgram(program.handle);
        program.handle = 0;
    }
    
    if (app->vao != 0) 
    {
        glDeleteVertexArrays(1, &app->vao);
        app->vao = 0;
    }

    if (app->embeddedVertices != 0)
    {
        glDeleteBuffers(1, &app->embeddedVertices);
        app->embeddedVertices = 0;
    }

    if (app->embeddedElements != 0)
    {
        glDeleteBuffers(1, &app->embeddedElements);
        app->embeddedElements = 0;
    }

    app->primaryFBO.Clean();
}

GLuint FindVAO(Mesh& mesh, u32 submeshIndex, const Program& program)
{
    Submesh& submesh = mesh.submeshes[submeshIndex];

    // Try finding a VAO for this submesh/program
    for (u32 i = 0; i < (u32)submesh.vaos.size(); ++i) {
        if (submesh.vaos[i].programHandle == program.handle) {
            return submesh.vaos[i].handle;
        }
    }

    GLuint vaoHandle = 0;

    // Create a new VAO for this submesh/program
    //CreateVAO(mesh, submesh, program, vaoHandle);
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferHandle);

    // Link all vertex input attributes to attributes in the vertex buffer
    for (u32 i = 0; i < program.vertexInputLayout.attributes.size(); ++i) {
        bool attributeWasLinked = false;

        for (u32 j = 0; j < submesh.vertexBufferLayout.attributes.size(); ++j) {
            if (program.vertexInputLayout.attributes[i].location == submesh.vertexBufferLayout.attributes[j].location) {
                const u32 index = submesh.vertexBufferLayout.attributes[j].location;
                const u32 ncomp = submesh.vertexBufferLayout.attributes[j].componentCount;
                const u32 offset = submesh.vertexBufferLayout.attributes[j].offset + submesh.vertexOffset; // attribute offset + vertex offset
                const u32 stride = submesh.vertexBufferLayout.stride;
                glVertexAttribPointer(index, ncomp, GL_FLOAT, GL_FALSE, stride, (void*)(u64)offset);
                glEnableVertexAttribArray(index);

                attributeWasLinked = true;
                break;
            }
        }

        assert(attributeWasLinked); // The submesh should provide an attribute for each vertex input
    }

    glBindVertexArray(0);
    Vao vao = { vaoHandle, program.handle };
    submesh.vaos.push_back(vao);

    return vaoHandle;
}

void CreateVAO(Mesh& mesh, Submesh& submesh, const Program& program, GLuint& vaoHandle)
{
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferHandle);

    // Link all vertex input attributes to attributes in the vertex buffer
    for (u32 i = 0; i < program.vertexInputLayout.attributes.size(); ++i) {
        bool attributeWasLinked = false;

        for (u32 j = 0; j < submesh.vertexBufferLayout.attributes.size(); ++j) {
            if (program.vertexInputLayout.attributes[i].location == submesh.vertexBufferLayout.attributes[j].location) {
                const u32 index = submesh.vertexBufferLayout.attributes[j].location;
                const u32 ncomp = submesh.vertexBufferLayout.attributes[j].componentCount;
                const u32 offset = submesh.vertexBufferLayout.attributes[j].offset + submesh.vertexOffset; // attribute offset + vertex offset
                const u32 stride = submesh.vertexBufferLayout.stride;
                glVertexAttribPointer(index, ncomp, GL_FLOAT, GL_FALSE, stride, (void*)(u64)offset);
                glEnableVertexAttribArray(index);

                attributeWasLinked = true;
                break;
            }
        }

        assert(attributeWasLinked); // The submesh should provide an attribute for each vertex input
    }

    glBindVertexArray(0);
}

OpenGLInfo GetOpenGLInfo(OpenGLInfo& glInfo)
{
    glInfo.version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    glInfo.renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    glInfo.vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    glInfo.glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Get extensions
    GLint numExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    glInfo.extensions.reserve(numExtensions);
    for (int i = 0; i < numExtensions; ++i) {
        const GLubyte* extension = glGetStringi(GL_EXTENSIONS, i);
        glInfo.extensions.emplace_back(reinterpret_cast<const char*>(extension));
    }

    return glInfo;
}

void InfoWindow(App* app)
{
    ImGui::Begin("Info");
    ImGui::Spacing();
    ImGui::Text("FPS: %f", 1.0f / app->deltaTime);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGuiCameraTab(app); // Camera 

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGuiGbufferTab(app); // GBuffer

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGuiLightTab(app); // Lights

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGuiGLInfoTab(app); // OpenGl Info

    ImGui::End();
}

void ImGuiCameraTab(App* app)
{
    if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // --- Position --- //
        ImGui::Spacing();

        glm::vec3 pos = app->camera.GetPosition();

        ImGui::TextColored(ImVec4(1, 0.5, 0.5, 1), "Position");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.3, 0.3, 1));
        ImGui::Text("X: %8.3f", pos.x);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 1, 0.3, 1));
        ImGui::Text("Y: %8.3f", pos.y);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.5, 1, 1));
        ImGui::Text("Z: %8.3f", pos.z);
        ImGui::PopStyleColor();
        ImGui::Spacing();

        // --- Vectors --- //
        ImGui::Spacing();
        glm::vec3 xdir;
        xdir = app->camera.GetXvector();

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1, 0.5, 0.5, 1), "X Vector");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.3, 0.3, 1));
        ImGui::Text("X: %8.3f", xdir.x);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 1, 0.3, 1));
        ImGui::Text("Y: %8.3f", xdir.y);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.5, 1, 1));
        ImGui::Text("Z: %8.3f", xdir.z);
        ImGui::PopStyleColor();
        ImGui::Spacing();

        glm::vec3 ydir;
        ydir = app->camera.GetYvector();

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1, 0.5, 0.5, 1), "Y Vector");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.3, 0.3, 1));
        ImGui::Text("X: %8.3f", ydir.x);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 1, 0.3, 1));
        ImGui::Text("Y: %8.3f", ydir.y);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.5, 1, 1));
        ImGui::Text("Z: %8.3f", ydir.z);
        ImGui::PopStyleColor();
        ImGui::Spacing();

        glm::vec3 zdir;
        zdir = app->camera.GetZvector();

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1, 0.5, 0.5, 1), "Z Vector");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.3, 0.3, 1));
        ImGui::Text("X: %8.3f", zdir.x);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 1, 0.3, 1));
        ImGui::Text("Y: %8.3f", zdir.y);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.5, 1, 1));
        ImGui::Text("Z: %8.3f", zdir.z);
        ImGui::PopStyleColor();
        ImGui::Spacing();

        ImGui::TreePop();
    }
}

void ImGuiGbufferTab(App* app)
{
    if (ImGui::TreeNodeEx("Render", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Text("GBuffer Modes:");
        ImGui::Spacing();

        if (ImGui::BeginCombo(" ", app->GBufferItems[app->currentGBufferItem].c_str())) {
            for (int i = 0; i < app->GBufferItems.size(); i++) {
                const bool isSelected = (app->currentGBufferItem == i);
                if (ImGui::Selectable(app->GBufferItems[i].c_str(), isSelected)) {
                    app->currentGBufferItem = i;
                    // Handle the GBuffer change here (e.g., update shader uniform)
                    UpdateGBuffer(app);
                }

                // Set the initial focus when opening the combo
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::TreePop();
    }
}

void ImGuiLightTab(App* app)
{
    if (ImGui::TreeNodeEx("Lights", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        ImGui::Text("Number of Lights: %d", app->lights.size());

        ImGui::Spacing();

        if (ImGui::Button("Create Default Lighting"))
        {
            CreateDefaultLights(app);
        }
        ImGui::SameLine();
        if (ImGui::Button("Lighting Stress Test"))
        {
            CreateLightStressTest(app);
        }

        bool lightChanged = false;

        for (auto& light : app->lights)
        {
            ImGui::Text(light.name.c_str());
            vec3 checkVector;

            ImGui::PushID(&light);
            float color[3] = { light.color.x, light.color.y ,light.color.z };
            ImGui::DragFloat3("Color", color, 0.01, 0.0, 1.0);
            checkVector = vec3(color[0], color[1], color[2]);

            if (checkVector != light.color)
            {
                light.color = checkVector;
                lightChanged = true;
            }

            if (light.type == 0)
            {
                float direction[3] = { light.direction.x, light.direction.y ,light.direction.z };
                ImGui::DragFloat3("Direction", direction, 0.01, -1.0, 1.0);
                checkVector = vec3(direction[0], direction[1], direction[2]);

                if (checkVector != light.direction)
                {
                    light.direction = checkVector;
                    lightChanged = true;
                }
            }

            if (light.type == 1) 
            {
                float position[3] = { light.position.x, light.position.y ,light.position.z };
                ImGui::DragFloat3("Position", position);
                checkVector = vec3(position[0], position[1], position[2]);

                if (checkVector != light.position)
                {
                    light.position = checkVector;
                    lightChanged = true;
                }
            }

            ImGui::PopID();
            if (lightChanged)
            {
                app->UpdateLights(app);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        ImGui::TreePop();
    }
}

void ImGuiGLInfoTab(App* app)
{
    if (ImGui::TreeNode("OpenGL Info"))
    {
        ImGui::Text("Version: %s", app->glInfo.version.c_str());
        ImGui::Text("Renderer: %s", app->glInfo.renderer.c_str());
        ImGui::Text("Vendor: %s", app->glInfo.vendor.c_str());
        ImGui::Text("GLSL Version: %s", app->glInfo.glslVersion.c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Extensions (%d):", static_cast<int>(app->glInfo.extensions.size()));

        ImGui::BeginChild("##extensions", ImVec2(0, 200), true);
        for (const auto& ext : app->glInfo.extensions) {
            ImGui::Text("%s", ext.c_str());
        }
        ImGui::EndChild();

        ImGui::TreePop();
    }
}

