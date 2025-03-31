
#ifdef RENDER_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec2 aTangent;
layout(location = 4) in vec2 aBitangent;

struct Light {
    int type;
    vec3 color;
    vec3 direction;
    vec3 position;
};

layout(binding = 0, std140) uniform GlobalParams 
{
    vec3 uCameraPosition;
    int uLightCount;
    Light uLight[16];
};

layout(binding = 1, std140) uniform EntityParams 
{
    mat4 uWorldMatrix;
    mat4 uWorldViewProjectionMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out vec3 vViewDir;

void main()
{
    vTexCoord = aTexCoord;
    vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
    vNormal = vec3(uWorldMatrix * vec4(aNormal, 0.0));
    vViewDir = uCameraPosition - vPosition;
    gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

struct Light {
    int type;
    vec3 color;
    vec3 direction;
    vec3 position;
};

layout(binding = 0, std140) uniform GlobalParams 
{
    vec3 uCameraPosition;
    int uLightCount;
    Light uLight[16];
};

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vViewDir;

uniform sampler2D uTexture;

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormals;
layout(location = 2) out vec4 oPosition;
layout(location = 3) out vec4 oViewDir;

void main()
{
    vec3 returnColor = vec3(0.0);

    for (int i = 0; i < uLightCount; ++i) 
    {  
        switch(uLight[i].type)
        {
            case 0:
            // Directional Light

            break;

            case 1:
                returnColor += CalcPointLight(uLight[i], vNormal, vPosition, vViewDir);
            break;
        }
        
    }

    oColor = vec4(uTexture, vTexCoord);
    oNormals = vec4(vNormal, 0.0);
    oPosition = vec4(vPosition, 0.0);
    oViewDir = vec4(vViewDir, 0.0);
}

#endif
#endif