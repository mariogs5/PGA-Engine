
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

layout(location = 0) out vec4 oColor;

vec3 CalcPointLight(Light alight, vec3 aNormal, vec3 aPosition, vec3 aViewDir)
{
    vec3 lightDir = normalize(alight.position - aPosition);
    float diff = max(dot(aNormal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, aNormal);
    float spec = pow(max(dot(aViewDir, reflectDir), 0.0), 2.0);

    float distance = length(alight.position - aPosition);

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 ambient = alight.color * 0.2;
    vec3 diffuse = alight.color * diff;
    vec3 specular = 0.1 * spec * alight.color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(Light alight, vec3 aNormal, vec3 aViewDir)
{
    vec3 lightDir = normalize(-alight.direction);
    float diff = max(dot(aNormal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, aNormal);
    float spec = pow(max(dot(aViewDir, reflectDir), 0.0), 2.0);

    vec3 ambient = alight.color * 0.2;
    vec3 diffuse = alight.color * diff;
    vec3 specular = 0.1 * spec * alight.color;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 Color = texture(uColor, vTexCoord).rgb;
    vec3 Normal = texture(uNormals, vTexCoord).xyz;
    vec3 Depth = texture(uDepth, vTexCoord).xyz;
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
        case 2: oColor = vec4(Normal * 0.5 + 0.5, 1.0); break;
        case 3: oColor = vec4(Depth, 1.0); break;
        case 4: oColor = vec4(Position * 0.1, 1.0); break;
        case 5: oColor = vec4(ViewDir * 0.5 + 0.5, 1.0); break;
        default: oColor = vec4(1,0,1,1); // Error color
    }
}

#endif
#endif
