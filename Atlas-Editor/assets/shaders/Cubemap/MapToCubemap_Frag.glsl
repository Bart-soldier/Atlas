//--------------------------
// - Atlas Map To Cubemap -
// Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec3 v_TexCoords;

layout (binding = 0) uniform sampler2D equirectangularMap;

layout (location = 0) out vec4 o_Color;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
	vec2 texCoord = SampleSphericalMap(normalize(v_TexCoords));
    
    o_Color = vec4(texture(equirectangularMap, texCoord).rgb, 1.0);
}