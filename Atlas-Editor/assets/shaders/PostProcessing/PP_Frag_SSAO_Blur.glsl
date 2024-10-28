//--------------------------
// - Atlas Post-Processing -
// SSAO Blur Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

layout (location = 0) out vec4 o_Color;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(u_screenTexture, 0));

    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(u_screenTexture, v_TexCoords + offset).r;
        }
    }

    result /= (4.0 * 4.0);
    o_Color = vec4(vec3(result), 1.0);
}