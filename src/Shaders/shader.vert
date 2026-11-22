
#version 450
// Tells shaderc that this is a vertex shader, so the type can be infered.
#pragma shader_stage(vertex)

layout(set = 0, binding = 0) uniform GlobalData
{
    mat4 cameraView;
    mat4 cameraProjection;
    vec4 cameraPosition;

    vec4 lightSourcePosition;
    vec4 lightSourceColor;

    vec4 phongParameters;
} globalData;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragPosition;

void main()
{
    gl_Position = globalData.cameraProjection*globalData.cameraView*vec4(inPosition, 1.0);

    fragColor = inColor;
    fragNormal = normalize(inNormal);
    fragPosition = inPosition;
}
