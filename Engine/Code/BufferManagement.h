#ifndef BUFFER_UTILS_H
#define BUFFE�R_UTILS_H

#include "platform.h"

#include <glad/glad.h>
#include <cstring>
#include <cassert>

using u8 = unsigned char;
using u32 = unsigned int;

#ifdef NDEBUG
#define ASSERT(condition, message) ((void)0)
#else
#define ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "[ASSERT] %s\nArchivo: %s\nL�nea: %d\n", \
                        message, __FILE__, __LINE__); \
                std::abort(); \
            } \
        } while(0)
#endif

struct Buffer {
    u32 handle = 0;
    u32 size = 0;
    GLenum type = 0;
    u8* data = nullptr;
    u32 head = 0;
};

bool IsPowerOf2(u32 value);
u32 Align(u32 value, u32 alignment);
Buffer CreateBuffer(u32 size, GLenum type, GLenum usage);
void BindBuffer(const Buffer& buffer);
void MapBuffer(Buffer& buffer, GLenum access);
void UnmapBuffer(Buffer& buffer);
void AlignHead(Buffer& buffer, u32 alignment);
void PushAlignedData(Buffer& buffer, const void* data, u32 size, u32 alignment);

#define CreateConstantBuffer(size) CreateBuffer(size, GL_UNIFORM_BUFFER, GL_STREAM_DRAW)
#define CreateStaticVertexBuffer(size) CreateBuffer(size, GL_ARRAY_BUFFER, GL_STATIC_DRAW)
#define CreateStaticIndexBuffer(size) CreateBuffer(size, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW)

#define PushData(buffer, data, size) PushAlignedData(buffer, data, size, 1)
#define PushUInt(buffer, value) { u32 v = value; PushAlignedData(buffer, &v, sizeof(v), 4); }
#define PushVec3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))
#define PushVec4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))
#define PushMat3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))
#define PushMat4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))

#endif // BUFFER_UTILS_H

