
#ifdef TEXTURED_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

struct Light {
    int type;
    vec3 color;
    vec3 direction;
    vec3 position;
    int intensity;
    int range;
};

layout(binding = 0, std140) uniform GlobalParams 
{
    vec3 uCameraPosition;
    int uLightCount;
    Light uLight[16];
};

in vec2 vTexCoord;

uniform sampler2D uColor;
uniform sampler2D uNormals;
uniform sampler2D uDepth;
uniform sampler2D uPosition;
uniform sampler2D uViewDir;
uniform int uGBuffer;
uniform float uNear;
uniform float uFar;

layout(location = 0) out vec4 oColor;

vec3 CalcPointLight(Light alight, vec3 aNormal, vec3 aPosition, vec3 aViewDir)
{
    vec3 lightDir = normalize(alight.position - aPosition);
    vec3 viewDir = normalize(aViewDir);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // Diffuse
    float diff = max(dot(aNormal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    float spec = pow(max(dot(aNormal, halfwayDir), 0.0), 32.0);
    
    // Attenuation
    float distance = length(alight.position - aPosition);
    float attenuation = clamp(1.0 - distance/float(alight.range), 0.0, 1.0);
    attenuation *= attenuation * 1.0 / (distance * distance + 1.0);
    
    // Combine all components and apply attenuation & intensity
    vec3 ambient = alight.color * 0.1;
    vec3 diffuse = alight.color * diff * 0.8;
    vec3 specular = alight.color * spec * 0.5;
    
    vec3 result = (ambient + diffuse + specular) * attenuation;
    return result * float(alight.intensity);
}

vec3 CalcDirLight(Light alight, vec3 aNormal, vec3 aViewDir)
{
    vec3 lightDir = -normalize(alight.direction); // Ensure direction is normalized
    vec3 halfwayDir = normalize(lightDir + normalize(aViewDir));
    
    float diff = max(dot(aNormal, lightDir), 0.0);
    float spec = pow(max(dot(aNormal, halfwayDir), 0.0), 64.0);
    
    // Apply intensity to all components
    vec3 result = alight.color * (0.1 + diff * 0.7 + spec * 0.2);
    return result * float(alight.intensity);
}

void main()
{
    vec3 Color = texture(uColor, vTexCoord).rgb;
    vec3 Normal = texture(uNormals, vTexCoord).xyz;
    float Depth = texture(uDepth, vTexCoord).r;
    vec3 ViewDir = texture(uViewDir, vTexCoord).xyz;
    vec3 Position = texture(uPosition, vTexCoord).xyz;

    switch(uGBuffer) {
        case 0: // Final render
        
            vec3 returnColor = vec3(0.0);
            for(int i = 0; i < uLightCount; ++i)
            {
                vec3 lightResult = vec3(0.0);
                if(uLight[i].type == 0)
                {
                    lightResult += CalcDirLight(uLight[i], Normal, ViewDir);
                    // lightResult * SSAO
                }
                else if(uLight[i].type == 1)
                {
                    lightResult += CalcPointLight(uLight[i], Normal, Position, ViewDir);
                }
                returnColor += lightResult * Color;
            }

            oColor = vec4(returnColor, 1.0);
            break;

        case 1: oColor = vec4(Color, 1.0); break;
        case 2: oColor = vec4(Normal, 1.0); break;
        case 3: 

            float linearDepth = (uNear * uFar) / (uFar - Depth * (uFar - uNear));
    
            float maxVisualizationDepth = 40.0;
            float normalizedDepth = linearDepth / maxVisualizationDepth;
    
            normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
            normalizedDepth = pow(normalizedDepth * 1.5, 0.7);
    
            oColor = vec4(vec3(1.0 - normalizedDepth), 1.0);
            break;

        case 4: oColor = vec4(Position, 1.0); break;
        case 5: oColor = vec4(ViewDir, 1.0); break;
        default: oColor = vec4(1,0,1,1); // Error color
    }
}

#endif
#endif
