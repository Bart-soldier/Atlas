//--------------------------
// - Atlas Post-Processing -
// Edge Detection Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

layout (std140, binding = 3) uniform Settings
{
	float u_Strength;
	float u_KernelOffset;
};

layout (location = 2) out vec4 o_PostProcessColor;

void main()
{
    float offset = 1.0 / u_KernelOffset;

    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // top-left
        vec2(0.0f, offset), // top-center
        vec2(offset, offset), // top-right
        vec2(-offset, 0.0f),   // center-left
        vec2(0.0f, 0.0f),   // center-center
        vec2(offset, 0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f, -offset), // bottom-center
        vec2(offset, -offset)  // bottom-right    
        );

    float kernel[9] = float[](
        1,  1, 1,
        1, -8, 1,
        1,  1, 1
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(u_screenTexture, v_TexCoords + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++)
    {
        color += sampleTex[i] * kernel[i];
    }

    o_PostProcessColor = vec4(color, 1.0);
}