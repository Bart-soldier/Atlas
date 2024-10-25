//--------------------------
// - Atlas Post-Processing -
// Gaussian Blur Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

layout (std140, binding = 3) uniform Settings
{
	float u_Strength; // > 0 is horizontal pass, <= 0 is vertical pass
	float u_KernelOffset;
};

layout (location = 0) out vec4 o_Color;

void main()
{
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    vec2 texOffset = 1.0 / textureSize(u_screenTexture, 0);
    vec3 blur = texture(u_screenTexture, v_TexCoords).rgb * weight[0];

    if(u_Strength > 0)
    {
        for(int i = 1; i < 5; ++i)
        {
            blur += texture(u_screenTexture, v_TexCoords + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
            blur += texture(u_screenTexture, v_TexCoords - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            blur += texture(u_screenTexture, v_TexCoords + vec2(0.0, texOffset.y * i)).rgb * weight[i];
            blur += texture(u_screenTexture, v_TexCoords - vec2(0.0, texOffset.y * i)).rgb * weight[i];
        }
    }

    o_Color = vec4(blur, 1.0);
}